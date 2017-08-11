#include "bessel.h"
#include "unittest.h"

#include "test_bessels.h"

int test_besselI()
{
    unittest_t test;
    unittest_init(&test, "Bessel function I_nu", "Test modified Bessel function I_nu", 1e-10);

    AssertAlmostEqual(&test, bessel_lnInu(0,1e-6), -7.133546631626697);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e-5), -5.982254085113174);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e-4), -4.830961536966152);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e-3), -3.679668825469134);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e-2), -2.528359779027661);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e-1), -1.375417787678169);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e0),  -0.064351991073531);
    AssertAlmostEqual(&test, bessel_lnInu(0,5e0),   3.276297109617906);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e1),   7.929768918237150);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e2),   96.77847637380128);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e3),   995.6271838273042);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e4),   9994.475891280807);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e5),   99993.32459873431);
    AssertAlmostEqual(&test, bessel_lnInu(0,1e6),   999992.1733061878);

    AssertAlmostEqual(&test, bessel_lnInu(1,1e-6), -22.04766947825915);
    AssertAlmostEqual(&test, bessel_lnInu(1,1),    -1.225791352644727);
    AssertAlmostEqual(&test, bessel_lnInu(1,3),     1.131235470744604);
    AssertAlmostEqual(&test, bessel_lnInu(1,1e6),   999992.1733051878);

    AssertAlmostEqual(&test, bessel_lnInu(2,1e-6), -37.47261794865755);
    AssertAlmostEqual(&test, bessel_lnInu(2,1),    -2.862970265776753);
    AssertAlmostEqual(&test, bessel_lnInu(2,5),     2.622265862896675);
    AssertAlmostEqual(&test, bessel_lnInu(2,1e6),   999992.1733031878);

    AssertAlmostEqual(&test, bessel_lnInu(3,1), -4.824473578629219);

    AssertAlmostEqual(&test, bessel_lnInu(23,1e-6), -394.1439513814884);
    AssertAlmostEqual(&test, bessel_lnInu(23,5),    -31.40382021014728);
    AssertAlmostEqual(&test, bessel_lnInu(23,1e6),   999992.1730301876);

    AssertAlmostEqual(&test, bessel_lnInu(119,1e-6), -2189.202200199878);
    AssertAlmostEqual(&test, bessel_lnInu(119,0.5),  -621.0792579289692);
    AssertAlmostEqual(&test, bessel_lnInu(119,3),    -406.9458492626251);
    AssertAlmostEqual(&test, bessel_lnInu(119,30),   -129.9524456900199);
    AssertAlmostEqual(&test, bessel_lnInu(119,300),   272.6929318295042);
    AssertAlmostEqual(&test, bessel_lnInu(119,1e6),   999992.1661661842);

    AssertAlmostEqual(&test, bessel_lnInu(702,1e-6),  -14098.666835519577122094);
    AssertAlmostEqual(&test, bessel_lnInu(702,1e-4),  -10863.534779862939382744);
    AssertAlmostEqual(&test, bessel_lnInu(702,3),     -3621.4923374733442116413);
    AssertAlmostEqual(&test, bessel_lnInu(702,1234),   1034.4300403851143436433);
    AssertAlmostEqual(&test, bessel_lnInu(702,12345),  12319.387046237228462572);

    AssertAlmostEqual(&test, bessel_lnInu(1000,1e-6), -20431.49449839618284504067476857469083458559739000691329331491527130271494692415150511174530433321433136);
    AssertAlmostEqual(&test, bessel_lnInu(1000,1e-3), -13520.28534177430503304703732555770536859193039475630981574458111516287899241469397029910642664433847926);
    AssertAlmostEqual(&test, bessel_lnInu(1000,1e-2), -11216.54895620904939558325377146323337831273555478835379834250875028435403387276682194621363431826046185);
    AssertAlmostEqual(&test, bessel_lnInu(1000,1e-1), -8912.812568197213596709615932332078627838011807630225365847030793958432903503595693734800910969123108949);
    AssertAlmostEqual(&test, bessel_lnInu(1000,0.5),  -7302.56987689676336897185629547968563842185599348104851731012744552380579915803449762971542175940010311);
    AssertAlmostEqual(&test, bessel_lnInu(1000,1),    -6609.075935527395980095353510794756629634116133600495644576672711499531490620620205721104566777718151736);
    AssertAlmostEqual(&test, bessel_lnInu(1000,2),    -5915.581432500951906286933113830174475212197542812903278574505170233704618286689873199126576488624484759);
    AssertAlmostEqual(&test, bessel_lnInu(1000,3),    -5509.912343712945267328979132190846725827912585175241766323863645387105816317555160947497391435769937895);
    AssertAlmostEqual(&test, bessel_lnInu(1000,1e3),  527.8529868786811522194044160531605993285935794084123362983354851474456335452024616554183049996262857132);
    AssertAlmostEqual(&test, bessel_lnInu(1000,1e6),  999991.6728059793130943353171944930595770676683137376427031951094117728445914870010390100004204380158778);
    AssertAlmostEqual(&test, bessel_lnInu(1000,1e10), 9999999987.568085951825096335671462781601752838390393537295559809872338563507206847874714997664267726706);
    AssertAlmostEqual(&test, bessel_lnInu(1000,1e15), 999999999999981.8116732688394846280847341032116508448801327783132111833491683317402934973256801484150616);

    AssertAlmostEqual(&test, bessel_lnInu(2000,1e-6), -42234.8947951300346830473379585826707214203079538014332120868066694902880518301183751383921520494969425);
    AssertAlmostEqual(&test, bessel_lnInu(2000,1e-3), -28415.93035952614447204783470568467897718070211006085666602823655701071729855636012138513811122976041258);
    AssertAlmostEqual(&test, bessel_lnInu(2000,1e-2), -23809.60888097919035546416104198835903437144416816511488244379844717366750919453532138465305412233327301);
    AssertAlmostEqual(&test, bessel_lnInu(2000,1e-1), -19203.28740120802932499732560256073212681904982339600229932769758180770283967628575962492378758150242858);
    AssertAlmostEqual(&test, bessel_lnInu(2000,0.5),  -15983.60682740609501738625934897628044319462397614645244426055481565428756661146279952563523515648114739);
    AssertAlmostEqual(&test, bessel_lnInu(2000,1),    -14596.96579901618788310022105892285226886336166936975392300752135695990335334932173801717581886652626646);
    AssertAlmostEqual(&test, bessel_lnInu(2000,2),    -13210.32448958711494509221620561379881786561635437662305625305769826121572416989512540545545030631885204);
    AssertAlmostEqual(&test, bessel_lnInu(2000,3),    -12399.19091628538400901116191205848297725854953927680784783263266711406481235666137863730296632293393371);
    AssertAlmostEqual(&test, bessel_lnInu(2000,1e3),  -656.7001407818153200995508884583794575393090890821986970759474019249446506053374294457329908466942456937);
    AssertAlmostEqual(&test, bessel_lnInu(2000,1e6),  999990.172305854646890506260540425580200718048383320566781239054287920583607231409451044892287670695543);
    AssertAlmostEqual(&test, bessel_lnInu(2000,1e10), 9999999987.567935901825088833797045489872753210108144726026175057069374821055689737724179572865461042364);
    AssertAlmostEqual(&test, bessel_lnInu(2000,1e15), 999999999999981.8116732673389846280847333529616514704628408616484212659313380072659381246512756849634249);

    AssertAlmostEqual(&test, bessel_lnInu(4000,1e-6), -87227.29694612307885103301200551584468893934317628631616701747428388257490491449595921078459951045462046);
    AssertAlmostEqual(&test, bessel_lnInu(4000,1e-3), -59592.82195255497683347496453777053702759223268379543711139413946158440566685472228673875391501650185858);
    AssertAlmostEqual(&test, bessel_lnInu(4000,1e-2), -50381.33028802611189400370358279645251952481474451288153310550083995161186129622431726580502281311627836);
    AssertAlmostEqual(&test, bessel_lnInu(4000,1e-1), -41169.83862288491394061415797214436124088635805386956508730435453197837201049901216731057019405676491579);
    AssertAlmostEqual(&test, bessel_lnInu(4000,0.5),  -34731.28223919791853593879425243173885979523852046869074163627971168402791815833718992748015213199550572);
    AssertAlmostEqual(&test, bessel_lnInu(4000,1),    -31958.34689651042931842410224045787624374748741273235419697098756105720591195121902889653317233701499943);
    AssertAlmostEqual(&test, bessel_lnInu(4000,2),    -29185.41141325066156495364073509034685044122651157141462840755182641012577013950373091666329274306323488);
    AssertAlmostEqual(&test, bessel_lnInu(4000,3),    -27563.34793588112522068641282910511475846540893261739747848932567748112963109709308894339741344193929689);
    AssertAlmostEqual(&test, bessel_lnInu(4000,1e3),  -4261.873145583658879013976460014704914159555190427781910931416108904940457318241683218735424515843188567);
    AssertAlmostEqual(&test, bessel_lnInu(4000,1e6),  999984.1713128587906335733650400189377789293783936556646686945153563664945363213862064612230031498868859);
    AssertAlmostEqual(&test, bessel_lnInu(4000,1e10), 9999999987.567335801825058838801709659040315205449860133337655607066253440553530455695896644350679577485);
    AssertAlmostEqual(&test, bessel_lnInu(4000,1e15), 999999999999981.8116732613379846280847303524616614751270070283451019295468175489710996535701378306006694);

    AssertAlmostEqual(&test, bessel_lnInu(8000,1e-6), -179983.9969950023818958723876995393909800188974763017267333454621001794193226738815355748703485777624212);
    AssertAlmostEqual(&test, bessel_lnInu(8000,1e-3), -124718.5008855057626382224931432319427460181572261050557869279392886859086270976588649925511516318932633);
    AssertAlmostEqual(&test, bessel_lnInu(8000,1e-2), -106296.6688490038069732059338608830975231912015585440088995771196622498066934297799163208258626503445598);
    AssertAlmostEqual(&test, bessel_lnInu(8000,1e-1), -87874.83681219562719759106262465804127002333030619443972456005074878148865262442015720990821889452308737);
    AssertAlmostEqual(&test, bessel_lnInu(8000,0.5),  -74998.5287862680135848981838044293745688477959626839724242180574439921344595337201466502864576912180495);
    AssertAlmostEqual(&test, bessel_lnInu(8000,1),    -69453.00474476506490151661304603788739112594518375770092839498060548029728159103977633136100975065922163);
    AssertAlmostEqual(&test, bessel_lnInu(8000,2),    -63907.48063296279819814166690502375466408213671760114476051934617734345137408812283483550973103169599278);
    AssertAlmostEqual(&test, bessel_lnInu(8000,3),    -60663.55687932272440755099250763165026480676393420076606234610233332977802891163333224305877662188500793);
    AssertAlmostEqual(&test, bessel_lnInu(8000,1e3),  -14156.32520283222190551846888640361239215109727877354379034198089153864533944898199947572101092991424095);
    AssertAlmostEqual(&test, bessel_lnInu(8000,1e6),  999960.1694608923673604843437031259973842720683280097870091709376176994502755612897910099990837427191856);
    AssertAlmostEqual(&test, bessel_lnInu(8000,1e10), 9999999987.564935601824938988839033039352579446614770672110687190549685675870570807497174476942723721488);
    AssertAlmostEqual(&test, bessel_lnInu(8000,1e15), 999999999999981.8116732373359846280847183514618215124503393621585472476518616826123922719706853820493953);

    AssertAlmostEqual(&test, bessel_lnInu(10000,1e-6), -227207.36475861054783856008);
    AssertAlmostEqual(&test, bessel_lnInu(10000,1e-3), -158126.3580911496612532687);
    AssertAlmostEqual(&test, bessel_lnInu(10000,1e-2), -135099.3558686602327614411);
    AssertAlmostEqual(&test, bessel_lnInu(10000,1e-1), -112072.3536459258160178544);
    AssertAlmostEqual(&test, bessel_lnInu(10000,0.5),  -95977.1697966294950886287);
    AssertAlmostEqual(&test, bessel_lnInu(10000,1),    -89045.3513986925741292707);
    AssertAlmostEqual(&test, bessel_lnInu(10000,2),    -82113.5329445140898436890);
    AssertAlmostEqual(&test, bessel_lnInu(10000,3),    -78058.67900589714115977290);
    AssertAlmostEqual(&test, bessel_lnInu(10000,5),    -72950.1669554853593243779);
    AssertAlmostEqual(&test, bessel_lnInu(10000,10),   -66018.34670157712688395283);
    AssertAlmostEqual(&test, bessel_lnInu(10000,100),  -42991.09701933247061596824);
    AssertAlmostEqual(&test, bessel_lnInu(10000,1e3),  -19939.3796349118643436095);
    AssertAlmostEqual(&test, bessel_lnInu(10000,1e4),  5322.2616513614162591753);
    AssertAlmostEqual(&test, bessel_lnInu(10000,1e5),  99493.6876160609222646113);
    AssertAlmostEqual(&test, bessel_lnInu(10000,1e6),  999942.1686979240393323496);
    AssertAlmostEqual(&test, bessel_lnInu(10000,1e10), 9.9999999875631355018248492299e9);

    AssertAlmostEqual(&test, bessel_lnInu(15000,1e-6), -346884.73738256604911624286);
    AssertAlmostEqual(&test, bessel_lnInu(15000,1e-3), -243264.9543201944856019237);
    AssertAlmostEqual(&test, bessel_lnInu(15000,1e-2), -208725.02663273565348379534);
    AssertAlmostEqual(&test, bessel_lnInu(15000,1e-1), -174185.0989451134876990345);
    AssertAlmostEqual(&test, bessel_lnInu(15000,0.5),  -150042.72553564616);
    AssertAlmostEqual(&test, bessel_lnInu(15000,1),    -139645.1712411579552601941);
    AssertAlmostEqual(&test, bessel_lnInu(15000,2),    -129247.6169091734952851689);
    AssertAlmostEqual(&test, bessel_lnInu(15000,3),    -123165.4374716719752416889);
    AssertAlmostEqual(&test, bessel_lnInu(15000,5),    -115502.7974357302343670549);
    AssertAlmostEqual(&test, bessel_lnInu(15000,10),   -105105.2419038658490281129);
    AssertAlmostEqual(&test, bessel_lnInu(15000,100),  -70565.1492338324971479077);
    AssertAlmostEqual(&test, bessel_lnInu(15000,1e3),  -36008.7324374704525851165);
    AssertAlmostEqual(&test, bessel_lnInu(15000,1e4),  99.8919443972822056655);
    AssertAlmostEqual(&test, bessel_lnInu(15000,1e5),  98870.33959211895254351702);
    AssertAlmostEqual(&test, bessel_lnInu(15000,1e6),  999879.66785945419736156375);
    AssertAlmostEqual(&test, bessel_lnInu(15000,1e10), 9.9999999875568852518245384103e9);

    AssertAlmostEqual(&test, bessel_lnInu(20000,1e-6), -468260.9825990783062629478);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1e-3), -330102.4231417960616542922);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1e-2), -284049.56998936741354389595);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1e-1), -237996.7168368162621212485);
    AssertAlmostEqual(&test, bessel_lnInu(20000,0.5),  -205807.15386617826256241592);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1),    -191943.8636720147794773473);
    AssertAlmostEqual(&test, bessel_lnInu(20000,2),    -178080.5734497284056640039);
    AssertAlmostEqual(&test, bessel_lnInu(20000,3),    -169971.0684925157513445529);
    AssertAlmostEqual(&test, bessel_lnInu(20000,5),    -159754.3004043990556846011);
    AssertAlmostEqual(&test, bessel_lnInu(20000,10),   -145891.0092821802133615813);
    AssertAlmostEqual(&test, bessel_lnInu(20000,100),  -99838.0323894238319097556);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1e3),  -53772.80906658759048431808);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1e4),  -6518.6780319667604965558);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1e5),  97999.8035128123689749377);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1e6),  999792.1698727360827571563);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1e10), 9.9999999875481350018241054555e9);

    AssertAlmostEqual(&test, bessel_lnInu(20000,1.3e4),  -471.05012404796373609835);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1.32e4), -106.0270907740333577163);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1.33e4), 75.0388656179391757008);
    AssertAlmostEqual(&test, bessel_lnInu(20000,1.4e4),  1317.0157175728013665654);

    AssertAlmostEqual(&test, bessel_lnInu(25000,1e-3), -418203.1851240105);
    AssertAlmostEqual(&test, bessel_lnInu(25000,1e-2), -360637.4065066118);
    AssertAlmostEqual(&test, bessel_lnInu(25000,1e-1), -303071.62788911520);
    AssertAlmostEqual(&test, bessel_lnInu(25000,0.5),  -262834.87535690662);
    AssertAlmostEqual(&test, bessel_lnInu(25000,1),    -245505.84926181816);
    AssertAlmostEqual(&test, bessel_lnInu(25000,2),    -228176.82314423104762);
    AssertAlmostEqual(&test, bessel_lnInu(25000,3),    -218039.99265897588);
    AssertAlmostEqual(&test, bessel_lnInu(25000,5),    -205269.09649202383);
    AssertAlmostEqual(&test, bessel_lnInu(25000,10),   -187940.06965447994);
    AssertAlmostEqual(&test, bessel_lnInu(25000,100),  -130374.1920432219);
    AssertAlmostEqual(&test, bessel_lnInu(25000,1e3),  -72798.5160180833);
    AssertAlmostEqual(&test, bessel_lnInu(25000,1e4),  -14261.79761355520);
    AssertAlmostEqual(&test, bessel_lnInu(25000,1e5),  96884.164810400878019);
    AssertAlmostEqual(&test, bessel_lnInu(25000,1e6),  999679.67692427272211);
    AssertAlmostEqual(&test, bessel_lnInu(25000,1e10), 9.9999999875368847518e9);

    AssertAlmostEqual(&test, bessel_lnInu(30000,1e-3), -507310.6819632926);
    AssertAlmostEqual(&test, bessel_lnInu(30000,1e-2), -438231.9778809239);
    AssertAlmostEqual(&test, bessel_lnInu(30000,1e-1), -369153.2737984735);
    AssertAlmostEqual(&test, bessel_lnInu(30000,0.5),  -320869.33170449437);
    AssertAlmostEqual(&test, bessel_lnInu(30000,1),    -300074.56970785604);
    AssertAlmostEqual(&test, bessel_lnInu(30000,2),    -279279.80769246865664);
    AssertAlmostEqual(&test, bessel_lnInu(30000,3),    -267115.65167500502);
    AssertAlmostEqual(&test, bessel_lnInu(30000,5),    -251790.62741588675);
    AssertAlmostEqual(&test, bessel_lnInu(30000,10),   -230995.86480052938);
    AssertAlmostEqual(&test, bessel_lnInu(30000,100),  -161917.0782224020);
    AssertAlmostEqual(&test, bessel_lnInu(30000,1e3),  -92830.1257091648);
    AssertAlmostEqual(&test, bessel_lnInu(30000,1e4),  -22937.62616692948153);
    AssertAlmostEqual(&test, bessel_lnInu(30000,1e5),  95526.028879136452164);
    AssertAlmostEqual(&test, bessel_lnInu(30000,1e6),  999542.19182442157069);
    AssertAlmostEqual(&test, bessel_lnInu(30000,1e10), 9.9999999875231345018e9);

    AssertAlmostEqual(&test, bessel_lnInu(35000,1e-3), -597255.3855066758);
    AssertAlmostEqual(&test, bessel_lnInu(35000,1e-2), -516663.7559593370);
    AssertAlmostEqual(&test, bessel_lnInu(35000,1e-1), -436072.1264119282);
    AssertAlmostEqual(&test, bessel_lnInu(35000,0.5),  -379740.99475606422);
    AssertAlmostEqual(&test, bessel_lnInu(35000,1),    -355480.49685751895);
    AssertAlmostEqual(&test, bessel_lnInu(35000,2),    -331219.99894290293047);
    AssertAlmostEqual(&test, bessel_lnInu(35000,3),    -317028.51739085037);
    AssertAlmostEqual(&test, bessel_lnInu(35000,5),    -299149.36503194799);
    AssertAlmostEqual(&test, bessel_lnInu(35000,10),   -274888.86660306831);
    AssertAlmostEqual(&test, bessel_lnInu(35000,100),  -194297.1663445478);
    AssertAlmostEqual(&test, bessel_lnInu(35000,1e3),  -113698.4664001639);
    AssertAlmostEqual(&test, bessel_lnInu(35000,1e4),  -32406.82004157346);
    AssertAlmostEqual(&test, bessel_lnInu(35000,1e5),  93928.470171454912493);
    AssertAlmostEqual(&test, bessel_lnInu(35000,1e6),  999379.71800676309700);
    AssertAlmostEqual(&test, bessel_lnInu(35000,1e10), 9.9999999875068842518e9);

    AssertAlmostEqual(&test, bessel_lnInu(40000,1e-6), -964230.4687784055);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e-5), -872125.9137660972);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e-4), -780021.3587537889);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e-3), -687916.8037414806);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e-2), -595812.2487291716);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e-1), -503707.6937168014);
    AssertAlmostEqual(&test, bessel_lnInu(40000,0.5),  -439329.37249898123);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1),    -411603.13869830582);
    AssertAlmostEqual(&test, bessel_lnInu(40000,2),    -383876.90488356842986);
    AssertAlmostEqual(&test, bessel_lnInu(40000,3),    -367658.09779543897);
    AssertAlmostEqual(&test, bessel_lnInu(40000,5),    -347224.81733199121);
    AssertAlmostEqual(&test, bessel_lnInu(40000,10),   -319498.58306727071);
    AssertAlmostEqual(&test, bessel_lnInu(40000,100),  -227393.9661823314);
    AssertAlmostEqual(&test, bessel_lnInu(40000,200),  -199667.5448941066);
    AssertAlmostEqual(&test, bessel_lnInu(40000,300),  -183448.4253521177);
    AssertAlmostEqual(&test, bessel_lnInu(40000,500),  -163014.1450527221);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e3),  -135283.2243901093);
    AssertAlmostEqual(&test, bessel_lnInu(40000,2e3),  -107538.2486140323);
    AssertAlmostEqual(&test, bessel_lnInu(40000,3e3),  -91288.2243954580);
    AssertAlmostEqual(&test, bessel_lnInu(40000,5e3),  -70755.2118667826);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e4),  -42564.7254132375);
    AssertAlmostEqual(&test, bessel_lnInu(40000,2e4),  -13031.054325408570400);
    AssertAlmostEqual(&test, bessel_lnInu(40000,2.5e4), -2796.35164946775547395);
    AssertAlmostEqual(&test, bessel_lnInu(40000,2.6e4), -935.8800579079119903979);
    AssertAlmostEqual(&test, bessel_lnInu(40000,2.65e4), -24.5356979107368592822);
    AssertAlmostEqual(&test, bessel_lnInu(40000,2.7e4), 874.93478459503573614760);
    AssertAlmostEqual(&test, bessel_lnInu(40000,3e4),  6048.63031278917300913);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e5),  92094.975826805733172);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e6),  999192.25952732985895);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e10), 9.9999999874881340018e9);

    AssertAlmostEqual(&test, bessel_lnInu(45000,1e-6), -1.09005728552281e6);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1e-5), -986439.805045528);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1e-4), -882822.324568250);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1e-3), -779204.844090971);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1e-2), -675587.363613692);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1e-1), -571969.883136358);
    AssertAlmostEqual(&test, bessel_lnInu(45000,0.5),  -499544.372356535);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1),    -468352.402653580);
    AssertAlmostEqual(&test, bessel_lnInu(45000,2),    -437160.432938126);
    AssertAlmostEqual(&test, bessel_lnInu(45000,3),    -418914.300312928);
    AssertAlmostEqual(&test, bessel_lnInu(45000,5),    -395926.891741760);
    AssertAlmostEqual(&test, bessel_lnInu(45000,10),   -364734.921626320);
    AssertAlmostEqual(&test, bessel_lnInu(45000,100),  -261117.386150909);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1e3),  -157494.406199760);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1e4),  -53330.3174826237);
    AssertAlmostEqual(&test, bessel_lnInu(45000,2e4),  -20519.915350151194);
    AssertAlmostEqual(&test, bessel_lnInu(45000,2.5e4), -9298.7012300491606);
    AssertAlmostEqual(&test, bessel_lnInu(45000,2.98e4), -49.434341147809384);
    AssertAlmostEqual(&test, bessel_lnInu(45000,2.985e4), 41.071387705541099);
    AssertAlmostEqual(&test, bessel_lnInu(45000,3e4),  311.95888730566566);
    AssertAlmostEqual(&test, bessel_lnInu(45000,4e4),  16683.384232795321);
    AssertAlmostEqual(&test, bessel_lnInu(45000,5e4),  30862.226534266592);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1e5),  90029.3863709419);
    AssertAlmostEqual(&test, bessel_lnInu(45000,1e6),  998979.821063697);

    return test_results(&test, stderr);
}

int test_besselK()
{
    unittest_t test;
    unittest_init(&test, "Bessel function K_nu", "Test modified Bessel function K_nu", 1e-10);

    AssertAlmostEqual(&test, bessel_lnKnu(0,1e-6), 7.133545631626864);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e-5), 5.9822440851298415);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e-4), 4.830861538632819);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e-3), 3.6786689921357962);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e-2), 2.5183764456387734);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e-1), 1.2770838991417504);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e0), -0.7742086473552725);
    AssertAlmostEqual(&test, bessel_lnKnu(0,5e0), -5.5789276035723227);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e1), -10.925501193852295);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e2), -102.07679374034932);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e3), -1003.2280862868463);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e4), -10004.379378833343);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e5), -100005.53067137984);
    AssertAlmostEqual(&test, bessel_lnKnu(0,1e6), -1.0000066819639263e6);

    AssertAlmostEqual(&test, bessel_lnKnu(1,1e-6), 20.949057189590638);
    AssertAlmostEqual(&test, bessel_lnKnu(1,1), -0.08106146679532716);
    AssertAlmostEqual(&test, bessel_lnKnu(1,3),   -3.0358327192375464858953059975);
    AssertAlmostEqual(&test, bessel_lnKnu(1,1e6), -1.00000668196292633e6);

    AssertAlmostEqual(&test, bessel_lnKnu(2,1e-6), 35.863180036223355);
    AssertAlmostEqual(&test, bessel_lnKnu(2,1), 1.17170150170004);
    AssertAlmostEqual(&test, bessel_lnKnu(2,5), -5.036603312746961080665958204772);
    AssertAlmostEqual(&test, bessel_lnKnu(2,1e6), -1.0000066819609263389096196908e6);

    AssertAlmostEqual(&test, bessel_lnKnu(3,1), 2.8367092652889516);

    AssertAlmostEqual(&test, bessel_lnKnu(4,1e15), -1.00000000000001704359684e15);

    AssertAlmostEqual(&test, bessel_lnKnu(23,1e-6), 390.29380377977833);
    AssertAlmostEqual(&test, bessel_lnKnu(23,5), 27.5314997887589672718741222750056);
    AssertAlmostEqual(&test, bessel_lnKnu(23,1e6), -1.00000668168792647542217765299e6);

    AssertAlmostEqual(&test, bessel_lnKnu(119,1e-6), 2183.7257366479472175742539693253862993069);
    AssertAlmostEqual(&test, bessel_lnKnu(119,0.5), 615.6027856231534);
    AssertAlmostEqual(&test, bessel_lnKnu(119,3), 401.4690706673959);
    AssertAlmostEqual(&test, bessel_lnKnu(119,30), 124.44542144141829);
    AssertAlmostEqual(&test, bessel_lnKnu(119,300), -279.16349731660983);

    AssertAlmostEqual(&test, bessel_lnKnu(702,1e-4), 10856.28698728117152647293);
    AssertAlmostEqual(&test, bessel_lnKnu(702,3), 3614.24453577321548255948381274);
    AssertAlmostEqual(&test, bessel_lnKnu(702,1234), -1042.3815655681729711090061175312483747);
    AssertAlmostEqual(&test, bessel_lnKnu(702,12345), -12329.50281632819683895772331427);

    AssertAlmostEqual(&test, bessel_lnKnu(1000,1e-6),  20423.89309606159906635627);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,1e-3),  13512.68393943972082096386);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,1e-2),  11208.94755387441573291358);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,1e-1),  8905.211165857634910126568);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,0.5),   7294.968474437304432718383);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,1),     6601.474532693311622436143);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,2),     5907.980028168368669895212);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,3),     5502.310936882873879713131);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,1e3),  -535.8007129753599475405978);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,1e6),  -1.0000061814642183370632e6);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,1e10), -1.000000001128708406232e10);
    AssertAlmostEqual(&test, bessel_lnKnu(1000,1e15), -1.000000000000017043596e15);

    AssertAlmostEqual(&test, bessel_lnKnu(2000,1e-6), 42226.600495521177448019358477);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,1e-3), 28407.63605991728711208242551347410421605322776491231395994766125798524892054103998951208870236101566941);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,1e-2), 23801.31458137032062668084180759672838125845304252994590033488275479619557180398648170447583186524115216);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,1e-1), 19194.99310159792271442454732704228054213374690854862801051458867730979534557270899831231795565971211545);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,0.5),  15975.31252776600339467187608093312535707914077314634565366819007935807950002404905589483263480947744491);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,1),    14588.67149928239310903330676527722691987526678617877217204826692058691507675101390153701642247065026715);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,2),    13202.03018947850774122088800596258951347650541202564571649218071992060909237818511456811473669964448446);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,3),    12390.89661555209004650742558398621593005116780950542623991805073527084526073691001806428493739840729096);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,1e3),  648.294319366065518802034415498008594181123452365297432127603561720663061711156467581586370132262221493);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,1e6),  -1000004.680965594167105932469829845470103971654739671969866440575207748582971755542128727774922908502889);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,1e10), -10000000011.28693401232551099339419215777417205434467042931479407771019600402239567240065422784110458705);
    AssertAlmostEqual(&test, bessel_lnKnu(2000,1e15), -1000000000000017.043596842809615197771839295685291152554863325436267162553436531272920902463898238115756);

    AssertAlmostEqual(&test, bessel_lnKnu(4000,1e-6), 87218.30962431022872707203747929992341689627115078757177848541018420181526590496669853000535707948105043);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,1e-3), 59583.8346307421266782718303369979951835458053356439749608476403586468383025034290516617373116784943535);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,1e-2), 50372.34296621325864582366863368085614522477703460092789190542474635635856082101806963411345413657562882);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,1e-1), 41160.851301071751394744144810440457725959519868572163158466152688846943729807546819827611861320271251);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,0.5),  34722.29491737725786430966851273097193075748833612255469932574523800736790922639597009183131315162867547);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,1),    31949.35957466633700452248517140962507277118897675355142871180110375542887073319568004144104772358582534);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,2),    29176.42409131284269294289708416534127905998550700586323050651604552252792224738948137099308240196255211);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,3),    27554.36061378709545753675948156390940588399662776337484951699558318700165334748983116456619175217645893);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,1e3),  4252.855518809915147217968820348395014997068929496977491144980088072087695250392941164195951464505546452);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,1e6),  -999998.6799785992508216780632712006845397733590304818079222678338780362049522111419122668802123719085807);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,1e10), -10000000011.28633391232554100839885632093893464983705833223604589886676415462772993916825265553538117486);
    AssertAlmostEqual(&test, bessel_lnKnu(4000,1e15), -1000000000000017.043596836808615197771842296185301157219029492072919832170416072978083103878370155105839);

    AssertAlmostEqual(&test, bessel_lnKnu(8000,1e-6), 179974.3165885031130212062631583732160569413320828456966863695240312238087281376618442420202065316473419);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,1e-3), 124708.8204790064937557448527625045724297784342835417777254980193095072600742586453677603236466532522766);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,1e-2), 106286.9884425045373173874520233600229479668482575041222026565055968236455924213699671918283223289027354);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,1e-1), 87865.15640569628020768844114792572085475116556184305992040044475806470731324385473084800764666375979752);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,0.5),  74988.84837976679182932310959211131822487184722522655096322567026560336265464051048339731537286759014985);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,1),    69443.32433825798450326043131462844891767605154028295587019878032232025736935303482421552396506963482505);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,2),    63897.80022643228322984752900267750820056457238921724818190852011630584705061241136669332821657017258996);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,3),    60653.876472753151824967723539474451331101578374917535227209444149304196586903600665785492173132844005);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,1e3),  14146.63704520101825834842301132270867090113363513077426025787947954438895896688390230097798560738496729);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,1e6),  -999974.678150633867367634820800296254980383456585285837577189092897557328137152920908914483608081513447);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,1e10), -10000000011.28393371232566117843617960522880129134499273676722732703673595636403080796780635596923603828);
    AssertAlmostEqual(&test, bessel_lnKnu(8000,1e15), -1000000000000017.043596812806615197771854297185461194542361824926141174278460206619418746144675877299245);

    AssertAlmostEqual(&test, bessel_lnKnu(10000,1e-6), 227197.4612210592616688495);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,1e-3), 158116.45455359837507855865);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,1e-2), 135089.45233110894609178054);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,1e-1), 112062.45010837447985314296);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,0.5),  95967.266259076958616);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,1),    89035.4478611362884594976);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,2),    82103.62940694280567402843);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,3),    78048.7754683008594912995);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,5),    72940.26341780908566810198);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,10),   66008.44316352589095544334);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,100),  42981.19343178868290500336);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,1e3),  19929.4711226901158149482);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,1e4),  -5332.5117375047010916433);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,1e5),  -99505.89866436692901148560);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,1e6),  -999956.67740566506321851726);
    AssertAlmostEqual(&test, bessel_lnKnu(10000,1e10), -1.00000000112821336123257514295e10);

    AssertAlmostEqual(&test, bessel_lnKnu(15000,1e-6), 346874.4283965726270336922);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,1e-3), 243254.6453342010635171510);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,1e-2), 208714.71764674223117903731);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,1e-1), 174174.78995912004339574302);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,0.5),  150032.41654965218738934428);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,1),    139634.86225516231110355707);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,2),    129237.3079231711849063318);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,3),    123155.1284856585544927160);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,5),    115492.4884496812604353064);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,10),   105094.93291765021958580276);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,100),  70554.8402256188278985014);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,1e3),  35998.4212343259721086096);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,1e4),  -110.3847825250555911729);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,1e5),  -98882.5567908024475061801);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,1e6),  -999894.17662968756554208650);
    AssertAlmostEqual(&test, bessel_lnKnu(15000,1e10), -1.000000001127588336232606563488e10);

    AssertAlmostEqual(&test, bessel_lnKnu(20000,1e-6), 468250.38593934552268438469);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1e-3), 330091.8264820632780744792);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1e-2), 284038.9733296346298403391);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1e-1), 237986.12017708346604331026);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,0.5),  205796.55720644516649947651);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1),    191933.2670122807459612802);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,2),    178069.9767899906223354438);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,3),    169960.4718327717183285671);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,5),    159743.7037446350236693777);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,10),   145880.4126223224360480948);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,100),  99827.4357171918295082837);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1e3),  53762.21115847704631843355);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1e4),  6507.9698054580073276562);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1e5),  -98012.0291967760160111874);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1e6),  -999806.6787304446136418366);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1e10), -1.00000000112671331123265077051e10);

    AssertAlmostEqual(&test, bessel_lnKnu(20000,1.3e4),  460.27726379659114357858);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1.32e4), 95.24964718639168430566);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1.33e4), -85.81861121543147227709);
    AssertAlmostEqual(&test, bessel_lnKnu(20000,1.4e4),  -1327.8117571444902965371);

    AssertAlmostEqual(&test, bessel_lnKnu(25000,1e-3), 418192.36532572625146);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,1e-2), 360626.58670832761223);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,1e-1), 303060.80809083096103);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,0.5),  262824.05555862219275);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,1),    245495.02946353313201);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,2),    228166.00334594362129);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,3),    218029.17286068445783);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,5),    205258.27669371960924);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,10),   187929.24985611571331);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,100),  130363.37223693803977);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,1e3),  72787.695420470330028);
    AssertAlmostEqual(&test, bessel_lnInu(25000,1e4),  -14261.79761355520048);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,1e5),  -96896.4011965338);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,1e6),  -999694.18589442612296);
    AssertAlmostEqual(&test, bessel_lnKnu(25000,1e10), -1.0000000011255882862e10);

    AssertAlmostEqual(&test, bessel_lnKnu(30000,1e-3), 507299.67984678481029);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,1e-2), 438220.97576441610175);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,1e-1), 369142.27168196574595);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,0.5),  320858.32958798648069);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,1),    300063.56759134773947);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,2),    279268.80557595868575);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,3),    267104.64955849231968);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,5),    251779.62529936515880);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,10),   230984.86268396611939);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,100),  161906.07610033894687);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,1e3),  92819.123037428381099);
    AssertAlmostEqual(&test, bessel_lnInu(30000,1e4),  -22937.626166929481533);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,1e5),  -95538.2780420063);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,1e6),  -999556.70093197270286);
    AssertAlmostEqual(&test, bessel_lnKnu(30000,1e10), -1.0000000011242132612e10);

    AssertAlmostEqual(&test, bessel_lnKnu(35000,1e-3), 597244.22924186915619);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,1e-2), 516652.59969453033119);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,1e-1), 436060.97014712151260);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,0.5),  379729.83849125747340);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,1),    355469.34059271189063);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,2),    331208.84267809464971);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,3),    317017.36112604004677);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,5),    299138.20876713114164);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,10),   274877.71033822084621);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,100),  194286.01007565965909);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,1e3),  113687.30972737214318);
    AssertAlmostEqual(&test, bessel_lnInu(35000,1e4),  -32406.8200415734610989);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,1e5),  -93940.7340248298);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,1e6),  -999394.22727664424969);
    AssertAlmostEqual(&test, bessel_lnKnu(35000,1e10), -1.0000000011225882362e10);

    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e-6), 964219.17898399197939);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e-5), 872114.62397168368544);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e-4), 780010.06895937533321);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e-3), 687905.51394706703307);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e-2), 595800.95893475806215);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e-1), 503696.40392238786332);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,0.5),  439318.08270456761849);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1),    411591.84890389197020);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,2),    383865.61508915364613);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,3),    367646.80800102262672);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,5),    347213.52753756985631);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,10),   319487.29327282591902);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,100),  227382.67638479291984);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e3),  135271.93428330112882);
    AssertAlmostEqual(&test, bessel_lnInu(40000,1e4),  -42564.725413234187470);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e5),  -92107.2561111780);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e6),  -999206.768984449);
    AssertAlmostEqual(&test, bessel_lnKnu(40000,1e10), -1.0000000011207132112e10);

    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e-6), 1.09004587794675E6);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e-5), 986428.397469468);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e-4), 882810.916992189);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e-3), 779193.436514911);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e-2), 675575.956037632);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e-1), 571958.475560298);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,0.5),  499532.964780474);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1),    468340.995077519);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,2),    437149.025362065);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,3),    418902.892736865);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,5),    395915.484165694);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,10),   364723.514050235);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,100),  261105.978572379);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e3),  157482.998376853);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,2e3),  126274.3669669763);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,3e3),  108000.478537718);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,4e3),  95015.8119430876);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,5e3),  84924.365910172593);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,6e3),  76658.921290801678);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,7e3),  69650.215607884471);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,8e3),  63558.47520617838);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,9e3),  58164.564730422659);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e4),  53318.885806035316);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,2e4),  20508.417645011621);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,3e4),  -323.55032233736438);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e5),  -90041.6846468200);
    AssertAlmostEqual(&test, bessel_lnKnu(45000,1e6),  -998994.330732934);

    return test_results(&test, stderr);
}
