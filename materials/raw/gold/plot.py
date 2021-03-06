import numpy as np
import matplotlib.pyplot as plt

# olmon_EV
olmonEV  = np.loadtxt("olmon_EV.csv", delimiter=",", usecols=(0,3))
olmonSC  = np.loadtxt("olmon_SC.csv", delimiter=",", usecols=(0,3))
olmonTS  = np.loadtxt("olmon_TS.csv", delimiter=",", usecols=(0,3))
palik    = np.loadtxt("palik.csv",    delimiter=",", usecols=(0,3))
werner_dft = np.loadtxt("werner_dft.csv",   delimiter=",", usecols=(0,3))
werner_exp = np.loadtxt("werner_exp.csv",   delimiter=",", usecols=(0,3))
hagemann = np.loadtxt("Hagemann.csv", delimiter=",", usecols=(0,3))
brandli  = np.loadtxt("brandli.csv",  delimiter=",", usecols=(0,3))

plt.loglog(olmonEV[:,0], olmonEV[:,1], "o", label="Olmon EV")
plt.loglog(olmonSC[:,0], olmonSC[:,1], "x", label="Olmon SC")
plt.loglog(olmonTS[:,0], olmonTS[:,1], "d", label="Olmon TS")
plt.loglog(werner_dft[:,0], werner_dft[:,1],  "+", label="Werner (DFT)")
plt.loglog(werner_exp[:,0], werner_exp[:,1],  "+", label="Werner (exp)")
plt.loglog(palik[:,0],   palik[:,1],   "+", label="Palik")
plt.loglog(hagemann[:,0],hagemann[:,1],"+", label="Hagemann")
plt.loglog(brandli[:,0], brandli[:,1], "+", label="Brandli")
plt.xlabel("omega [eV]")
plt.ylabel("eps''(omega)")
plt.legend()
plt.show()
