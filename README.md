# gut-bayesian

The code used to generate numerical results in [arXiv:1708.07835](https://arxiv.org/abs/1708.07835).

## Compiling and running

Compile with

```
gcc main.c -L/usr/lib/x86_64-linux-gnu -l SDL2 -l epoxy -l GL -l m -o gpu
```

And run with
```
./gpu
```

Of course gcc and needed libraries need to be installed.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

We ask that papers using code this cite [arXiv:1708.07835](https://arxiv.org/abs/1708.07835).

```
%\cite{Fundira:2017vip}
\bibitem{Fundira:2017vip} 
  P.~Fundira and A.~Purves,
  %``Bayesian naturalness, simplicity, and testability applied to the $B-L$ MSSM GUT using GPU Monte Carlo,''
  arXiv:1708.07835 [hep-ph].
  %%CITATION = ARXIV:1708.07835;%%
  %3 citations counted in INSPIRE as of 02 Feb 2018
```

## Authors

* **Austin Purves** - [AustinNH](https://github.com/AustinNH)

* **Panashe Fundira** - [munyari](https://github.com/munyari)

Please direct questions to <austin.purves@mville.edu>.