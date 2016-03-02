#include <ctype.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>

#include "casimir.h"
#include "floattypes.h"
#include "libcasimir.h"
#include "sfunc.h"
#include "utils.h"

/* default values for precision and lfac */
#define DEFAULT_PRECISION 1e-10
#define DEFAULT_LFAC 5
#define MIN_LMAX 20


/* print usage */
void usage(FILE *stream)
{
    char msg[4096];

    casimir_compile_info(msg, sizeof(msg)/sizeof(char));

    fprintf(stream, "Usage: casimir [OPTIONS]\n\
This program will calculate the free Casimir energy F(T,L/R) for the\n\
plane-sphere geometry for given L/R and temperature T. The output is in scaled\n\
units.\n\
\n\
Mandatory options:\n\
    -x, --LbyR L/R\n\
        Separation L between sphere and plane divided by radius of sphere,\n\
        where L/R > 0.\n\
        If you want to calculate several points, you may pass start and stop\n\
        value and the amount of points to be calculated.\n\
        Examples:\n\
            $ ./casimir -T 1 -x 0.5,0.9,5\n\
            This will calculate five free energies for Q=0.5,...,0,9 in linear\n\
            scale.\n\
            $ ./casimir -T 1 -x 0.5,0.9,5,log\n\
            This will calculate five free energies for Q=0.5,...,0,9, but using\n\
            a logarithmic scale.\n\
\n\
    -T TEMPERATURE\n\
        Temperature in units of hbar*c/(2pi*kB*(L+R)). You may use the same\n\
        syntax like for -x to calculate a range of points.\n\
\n\
Further options:\n\
    -g, --gamma\n\
        Set value of relaxation frequency gamma of Drude metals in units of\n\
        c/(L+R). If omitted, gamma = 0.\n\
\n\
    -w, --omegap\n\
        Set value of Plasma frequency omega_p of Drude metals in units of\n\
        c/omegaP. If omitted, omegap = INFINITY.\n\
\n\
    -l, --lscale\n\
        Specify parameter lscale. The vector space has to be truncated for\n\
        some value lmax. This program will use lmax=MAX(R/L*lscale, %d)\n\
        (default: %d)\n\
\n\
    -L LMAX\n\
        Set lmax to the value LMAX. When -L is specified, -l will be ignored\n\
\n\
    -c, --cores CORES\n\
        Use CORES of processors for the calculation (default: 1)\n\
\n\
    -p, --precision\n\
        Set precision to given value (default: %g)\n\
\n\
    --buffering\n\
        Enable buffering. By default buffering for stderr and stdout is\n\
        disabled.\n\
\n\
    -q, --quiet\n\
        The progress is printed to stderr unless this flag is set.\n\
\n\
    -h,--help\n\
        Show this help\n\
\n\
\n\
%s\n", MIN_LMAX, DEFAULT_LFAC, DEFAULT_PRECISION, msg);
}

/* parse a range given for LbyR or T from the command line.
 * Examples:
 * 1) "value"            => list = { value, value, 1, SCALE_LIN }
 * 2) "start,stop,N"     => list = { start, stop, N, SCALE_LIN }
 * 3) "start,stop,N,log" => list = { start, stop, N, SCALE_LOG }
 */
void parse_range(const char param, const char *_optarg, double list[])
{
    int elems = cinstr(_optarg, ','); /* commas in _optarg */
    list[3] = SCALE_LIN;

    switch(elems)
    {
        case 0:
            /* no comma => example 1) */
            list[0] = list[1] = atof(_optarg);
            list[2] = 1;
            break;
        case 3:
            /* 3 commas => example 3) */
            if(strncasecmp(indexn(_optarg, ',', 3)+1, "log", 3) == 0)
                list[3] = SCALE_LOG;
            /* here no break! */
        case 2:
            /* 2 commas => example 2) */
            list[0] = atof(_optarg);
            list[1] = atof(indexn(_optarg, ',', 1)+1);
            list[2] = atoi(indexn(_optarg, ',', 2)+1);

            /* N must be positive */
            if(list[2] <= 0)
            {
                fprintf(stderr, "error parsing parameter -%c\n\n", param);
                usage(stderr);
                exit(1);
            }

            /* ensure that start < stop */
            if(list[0] > list[1])
                swap(&list[0], &list[1]);
            break;

        default:
            fprintf(stderr, "Can't parse range %s.\n\n", _optarg);
            usage(stderr);
            exit(1);
    }
}


int main(int argc, char *argv[])
{
    double gamma_ = 0, omegap = 0;
    double precision = DEFAULT_PRECISION;
    double lfac      = DEFAULT_LFAC;
    double lT[4]     = { 0,0,0,SCALE_LIN }; /* start, stop, N, lin/log */
    double lLbyR[4]  = { 0,0,0,SCALE_LIN }; /* start, stop, N, lin/log */
    int i;
    int cores = 1;
    int lmax = 0;
    int buffering_flag = 0, quiet_flag = 0;

    /* parse command line options */
    while (1)
    {
        int c;
        struct option long_options[] =
        {
            { "quiet",     no_argument,       &quiet_flag,     1 },
            { "buffering", no_argument,       &buffering_flag, 1 },
            { "help",      no_argument,       0, 'h' },
            { "LbyR",      required_argument, 0, 'x' },
            { "lscale",    required_argument, 0, 'l' },
            { "cores",     required_argument, 0, 'c' },
            { "precision", required_argument, 0, 'p' },
            { "gamma",     required_argument, 0, 'g' },
            { "omegap",    required_argument, 0, 'w' },
            { 0, 0, 0, 0 }
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;
      
        c = getopt_long (argc, argv, "x:T:c:s:a:l:L:p:g:w:Xqh", long_options, &option_index);
      
        /* Detect the end of the options. */
        if(c == -1)
            break;
      
        switch (c)
        {
            case 0:
                /* If this option sets a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
            case 'x':
                parse_range('x', optarg, lLbyR);
                break;
            case 'T':
                parse_range('T', optarg, lT);
                break;
            case 'L':
                lmax = atoi(optarg);
                break;
            case 'q':
                quiet_flag = 1;
                break;
            case 'c':
                cores = atoi(optarg);
                break;
            case 'l':
                lfac = atof(optarg);
                break;
            case 'p':
                precision = atof(optarg);
                break;
            case 'g':
                gamma_ = atof(optarg);
                break;
            case 'w':
                omegap = atof(optarg);
                break;
            case 'h':
                usage(stdout);
                exit(0);
      
            case '?':
                /* getopt_long already printed an error message. */
                break;
      
            default:
                abort();
        }
    }

    // disable buffering
    if(!buffering_flag)
    {
        fflush(stdin);
        fflush(stderr);
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
    }

    /* check command line arguments */
    {
        if(lfac <= 0)
        {
            fprintf(stderr, "wrong argument for -l, --lscale: lscale must be positive\n\n");
            usage(stderr);
            exit(1);
        }
        if(lmax < 0)
        {
            fprintf(stderr, "wrong argument for -L: lmax must be positive\n\n");
            usage(stderr);
            exit(1);
        }
        if(precision <= 0)
        {
            fprintf(stderr, "wrong argument for -p, --precision: precision must be positive\n\n");
            usage(stderr);
            exit(1);
        }
        if(lLbyR[0] <= 0 || lLbyR[1] <= 0)
        {
            fprintf(stderr, "wrong argument for -x: x=L/R must be positive\n\n");
            usage(stderr);
            exit(1);
        }
        if(lT[0] <= 0)
        {
            fprintf(stderr, "wrong argument for -T: temperature must be positive\n\n");
            usage(stderr);
            exit(1);
        }
        if(cores < 1)
        {
            fprintf(stderr, "wrong argument for -c: number of cores must be >= 1\n\n");
            usage(stderr);
            exit(1);
        }
        if(gamma_ < 0)
        {
            fprintf(stderr, "wrong argument for --gamma: gamma must be nonnegative\n\n");
            usage(stderr);
            exit(1);
        }
        if(omegap < 0)
        {
            fprintf(stderr, "wrong argument for --omegap: omegap must be nonnegative\n\n");
            usage(stderr);
            exit(1);
        }
    }

    if(!quiet_flag)
    {
        char msg[4096];
        casimir_compile_info(msg, sizeof(msg)/sizeof(char));
        printf("# %s\n#\n", msg);
    }

    i = 0;
    for(int iLbyR = 0; iLbyR < lLbyR[2]; iLbyR++)
        for(int iT = 0; iT < lT[2]; iT++)
        {
            casimir_t casimir;
            double start_time = now();
            int nmax;
            double F,LbyR,T;

            if(lLbyR[3] == SCALE_LIN)
                LbyR = linspace(lLbyR[0], lLbyR[1], lLbyR[2], iLbyR);
            else
                LbyR = logspace(lLbyR[0], lLbyR[1], lLbyR[2], iLbyR);

            if(lT[3] == SCALE_LIN)
                T = linspace(lT[0], lT[1], lT[2], iT);
            else
                T = logspace(lT[0], lT[1], lT[2], iT);

            casimir_init(&casimir, LbyR, T);
            casimir_set_cores(&casimir, cores);
            casimir_set_precision(&casimir, precision);

            if(gamma_ > 0)
            {
                casimir_set_gamma_sphere(&casimir, gamma_);
                casimir_set_gamma_plane (&casimir, gamma_);
            }
            if(omegap > 0)
            {
                casimir_set_omegap_sphere(&casimir, omegap);
                casimir_set_omegap_plane (&casimir, omegap);
            }

            if(lmax > 0)
                casimir_set_lmax(&casimir, lmax);
            else
                casimir_set_lmax(&casimir, MAX((int)ceil(lfac/LbyR), MIN_LMAX));


            if(!quiet_flag)
            {
                casimir_info(&casimir, stdout, "# ");
                printf("#\n# LbyR, T, F, lmax, nmax, time\n");
            }

            F = casimir_F(&casimir, &nmax);
            casimir_free(&casimir);

            printf("%.15g, %.15g, %.15g, %d, %d, %g\n", LbyR, T, F, casimir.lmax, nmax, now()-start_time);

            if(!quiet_flag)
            {
                double progress = ++i*100/(lLbyR[2]*lT[2]);
                fprintf(stderr, "# %6.2f%%, L/R=%g, T=%g\n", progress, LbyR, T);
                if(progress != 100)
                    printf("#\n#\n");
            }
        }

    return 0;
}
