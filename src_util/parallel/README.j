+-------------------------------------------------------------------------+
| $B:n@.!'(B1992/7/10 $B;0I)(B $B>pEE8&(B $B8E;T>;0l(B (furuichi@isl.melco.co.jp)         |
| $B2~Dj!'(B                                                                  |
| $BMzNr(B                                                                    |
+-------------------------------------------------------------------------+

$BK\(B README.j $B$G$O!"0J2<$N(BICOT Free Software $B$K4X$9$k4JC1$J@bL@$r9T$$$^$9!#(B

        $B!V(BPIMOS$BIi2Y%P%i%s%9%f!<%F%#%j%F%#!W(B
        (1) $B%9%?%C%/J,3dF0E*Ii2Y%P%i%s%5!<(B (STB) 
        (2) $B%^%k%A%l%Y%kF0E*Ii2Y%P%i%s%5!<(B (MLB) 

1. $BK\%=%U%H%&%'%"$N4JC1$J@bL@(B
-----------------------------
  $BK\%=%U%H%&%'%"$O!"(BOR$BJBNs7?$NC5:wLdBj$KE,$7$?#2<oN`$NF0E*Ii2Y%P%i%s%9(B
$B%f!<%F%#%j%F%#$G$9!#K\%f!<%F%#%j%F%#%W%m%0%i%`$rMQ$$$k;v$K$h$j!"%f!<%6(B
$B$OC5:w%W%m%0%i%`$N5-=R$r9T$$!"Ii2Y%P%i%s%9$K4X$7$F$OK\%f!<%F%#%j%F%#$,(B
$B<+F0E*$K9T$C$F$/$l$^$9!#(B

  $BK\%=%U%H%&%'%"$O!"#2$D$N0[$J$kIi2Y%P%i%s%9J}<0MxMQ$7$F$$$^$9!#0J2<$K(B
$B$=$l$>$l$NFCD'$r4JC1$K@bL@$7$^$9$,!">\$7$/$O;29MJ88%$N(B[1,2,3,4] $B$r;2>H(B
$B$7$F2<$5$$!#(B

1.1 $B%9%?%C%/J,3dF0E*Ii2Y%P%i%s%5!<(B (STB)

  $B!VC5:wLdBj$NC5:wLZ$r%9%?%C%/$GI=8=$7!"%9%?%C%/$+$i%N!<%I%G!<%?$r0l$D(B
$B<h$j=P$7$F$O=hM}$r9T$$!"$=$N7k2L$r99$K%9%?%C%/$KLa$9$H$$$&=hM}$N7+$jJV(B
$B$7$GI=8=$G$-$k%?%$%W$NLdBj!W$NIi2YJ,;6$r<+F0E*$K9T$&$3$H$,$G$-$^$9!#(B

  $B%W%m%;%C%54V$NIi2Y%P%i%s%9$O!"K\%7%9%F%`$,<B9T;~$K<+F0E*$K<!$N$h$&$K(B
$B9T$$$^$9!#%W%m%;%C%5$,2K$K$J$C$?!J%9%?%C%/Fb$N%N!<%I%G!<%?$,6u$K$J$C$?!K(B
$B;~$K$O!"B>$N%W%m%;%C%5>e$N%9%?%C%/$+$i%G!<%?$r<h$C$FMh$^$9!#A4$F$N%W%m(B
$B%;%C%5>e$N%N!<%I%G!<%?$,6u$K$J$C$?;~$,<B9T$N=*N;$G$9$,!"$=$NH=Dj$bK\%7(B
$B%9%F%`$,<+F0E*$K9T$$$^$9!#(B

  $B%f!<%6$,K\%P%i%s%5!<$r;H$&$K$O!"<!$N#2$D$N=R8l$r%b%8%e!<%k(B 
stb_solver $BCf$KDj5A$7$^$9!#(B

  expand(NodeData,^SubNodeData,^Solutions)
        $BK\=R8l$O!"%7%9%F%`$,%9%?%C%/$+$i(B NodeData $B$r<h$j=P$7$?(B
        $B;~$K8F$P$l$^$9!#K\%G!<%?$r=hM}$7$?8e<!$K%9%?%C%/$K@Q$`(B
        $B%G!<%?$r(B SubNodeData $B$K!"2r$r(B Solutions $B$KJV$9$h$&$J=R(B
        $B8l$H$7$FDj5A$7$F2<$5$$!#(B

  combine(In,Out)
        $BK\=R8l$O!"3F%W%m%;%C%5Fb$GF@$i$l$?2r(B(Solutions)$B$KBP$7$F(B
        $B%U%#%k%?!<$r$+$1$k$?$a$N=R8l$G!"%7%9%F%`$,<+F0E*$K3F%W(B
        $B%m%;%C%5>e$K0l$D$:$D:n@.$7$^$9!#3F%W%m%;%C%5$+$i$NK\=R(B
        $B8l$N=PNO(B Out $B$O!"AH9~=R8l(B merge $B$K$h$C$F%^!<%8$5$l$?8e(B
        $B%f!<%6$KJV$5$l$^$9!#(B
        $BK\=R8l$O!"Nc$($P<!$N$h$&$KDj5A$7$^$9!#(B

        $B!&F@$i$l$?2r$NAmOB$r7W;;$9$k>l9g(B
                combine(In,Out):- true   | sum(In,0,Out).
                sum([],    S,Out):- true | Out=[S].
                sum([X|Xs],S,Out):- true | sum(Xs,~(S+X),Out).

        $B!&F@$i$l$?2r$r%^!<%8$9$k(B]\ \\
                combine(In,Out):- true | merge(In,Out).

STB $B%f!<%F%#%j%F%#$G$"$k%b%8%e!<%k(B stb ($B%=!<%9%U%!%$%kL>(B STB.kl1) $B$r%3(B
$B%s%Q%$%k$"$k$$$O%m!<%I$7$?8e!"%f!<%6$N(B stb_solver $B%b%8%e!<%k$r%3%s%Q%$(B
$B%k$7!":F%j%s%/$9$k$3$H$K$h$C$FMxMQ$7$^$9!#(B

$B0J>e$G(BSTB$B$N@bL@$O=*$j$G$9$,!"99$KIi2Y%P%i%s%9J}<0$K$D$$$FCN$j$?$$>l9g$O(B
$B;29MJ88%$N(B [3,4] $B$r;2>H$7$F2<$5$$!#$^$?!"<B:]$K<+J,$N%W%m%0%i%`$KE,MQ$9(B
$B$k>l9g$K$O!"Bh(B4$B>O$N%$%s%9%H!<%k<j=g$"$k$$$O;29MJ88%$N(B [4] $B$r;2>H$7$F2<(B
$B$5$$!#(B

1.2 $B%^%k%A%l%Y%kF0E*Ii2Y%P%i%s%5!<(B (MLB)

  $B!VC5:wLdBj$NC5:wLZ$r%W%m%;%9$N%U%)!<%/$GI=8=$7!"%W%m%;%9$r%U%)!<%/$9(B
$B$kItJ,$KIi2YJ,;6%W%i%0%^$rIU$1$k%?%$%W$NLdBj!W(B $B$NIi2YJ,;6$KE,$7$^$9!#K\(B
$B%f!<%F%#%j%F%#$G$O!"%W%m%;%9$r%W%m%;%C%5$K%^%C%T%s%0$9$k:]$K!"8=:_2K$J(B
$B%W%m%;%C%5$r65$($k5!G=$rDs6!$7$^$9!#(B

  $B%W%m%;%C%54V$NIi2Y%P%i%s%9$O!"2K$J%W%m%;%C%5$KBP$7$F%W%m%;%9$r%^%C%T(B
$B%s%0$9$k$3$H$K$h$C$F9T$$$^$9$,!"%W%m%;%9$r@8@.$9$k%W%m%;%C%5$,0lBf$@$C(B
$B$?>l9g$K$O!"%W%m%;%C%5$NBf?t$,A}$($?;~$K@8@.%W%m%;%C%5$,%\%H%k%M%C%/$H(B
$B$J$j$^$9!#=>$C$F!"%W%m%;%C%5$N%^%C%T%s%0$r3,AXE*$K%^%k%A%l%Y%k$G9T$&$3(B
$B$H$K$h$C$F!"Bf?t$N3HD%@-(B($B%9%1!<%i%S%j%F%#(B)$B$r9b$/$7$F$$$^$9!#(B

$B0J>e$G(B MLB $B$N@bL@$O=*$j$G$9$,!"99$KIi2Y%P%i%s%9J}<0$K$D$$$FCN$j$?$$>l9g(B
$B$O;29MJ88%$N(B [1,2,4] $B$r;2>H$7$F2<$5$$!#$^$?!"<B:]$K<+J,$N%W%m%0%i%`$KE,(B
$BMQ$9$k>l9g$K$O!"Bh(B 4 $B>O$N%$%s%9%H!<%k<j=g$"$k$$$O;29MJ88%$N(B [4] $B$r;2>H(B
$B$7$F2<$5$$!#(B


2. $B%W%m%0%i%`$N<B9T4D6-(B
-----------------------

 1. $BF0:n$,3NG'$5$l$F$$$k%7%9%F%`(B
        STB: $B%^%k%A(BPSI, PIM/m
        MLB: $B%^%k%A(BPSI, PIM/m, PIM/p
 2. $BF0:n$,3NG'$5$l$F$$$k(B PIMOS $B$N%P!<%8%g%s(B
        PIMOS 3.0 $B0J>e(B

$B$J$*!"5?;w%^%k%A(BPSI$B$"$k$$$O5?;w(B PIM/m $B>e$G$bF0:n$O$7$^$9$,!"Ii2Y%P%i%s(B
$B%9$O@5$7$/$*$3$J$o$l$^$;$s!#(B


3. $B%U%!%$%kL>0lMw(B
-----------------

$B!&%9%?%C%/J,3dF0E*Ii2Y%P%i%s%5!<(B STB:
    $B%=!<%9%U%!%$%k(B: STB.kl1         ($B%b%8%e!<%k(B stb )
                    STB_SOL.kl1     ($B%b%8%e!<%k(B stb_solver)

        $B$J$*!"(BSTB_SOL.kl1 $B$O%@%_!<$GF~$C$F$$$k%W%m%0%i%`$G$"$j!"(B
        $B%f!<%6$,<B:]$KMxMQ$9$k:]$K$OF1$8L>A0$N%b%8%e!<%kCf$K(B
        expand/3 $B$H(B combine/2 $B$rDj5A$9$kI,MW$,$"$j$^$9!#(B

$B!&%^%k%A%l%Y%kF0E*Ii2Y%P%i%s%5!<(B MLB:
    $B%=!<%9%U%!%$%k(B: MLB.kl1         ($B%b%8%e!<%k(B multi_level_load_balancer)

$B!&%I%-%e%a%s%H(B
        README.j        $BK\@bL@%U%!%$%k(B
        README          $BK\@bL@%U%!%$%k$N1Q8lHG(B
        INSTALL.j       $B%$%s%9%H!<%k>e$NCm0U;v9`!JF|K\8l!K(B
        INSTALL         $B%$%s%9%H!<%k>e$NCm0U;v9`!J1Q8l!K(B
        balman.ps       $B;29MJ88%(B [4] $B$NF|K\8l%]%9%H%9%/%j%W%H7A<0%U%!%$%k(B

4. $B%$%s%9%H!<%k<j=g(B
-------------------

$B!&%9%?%C%/J,3dF0E*Ii2Y%P%i%s%5!<(B (STB)

  $B%=!<%9%W%m%0%i%`$NF~$C$?(B MLB.kl1 $B$r%3%s%Q%$%k$7$^$9!#(B $B<B9T%*%V%8%'%/(B
  $B%H$rJ]B8$7$F$*$1$P!"<!2s0J9_$O%m!<%I$9$k$N$_$GMxMQ$G$-$^$9!#(B

$B!&%^%k%A%l%Y%kF0E*Ii2Y%P%i%s%5!<(B (MLB)

  $B%=!<%9%W%m%0%i%`$NF~$C$?(B STB.kl1 $B$H(B STB_SOL.kl1 $B$r%3%s%Q%$%k$7$^$9!#(B
  $B<B9T%*%V%8%'%/%H$rJ]B8$7$F$*$1$P!"<!2s0J9_$O%m!<%I$9$k$N$_$GMxMQ$G$-(B
  $B$^$9!#(B
  $B$J$*!"(BSTB_SOL.kl1 $B$O%@%_!<$N%b%8%e!<%k$G$"$j!"%f!<%6$,<B:]$KMxMQ$9$k(B
  $B:]$K$OF1$8L>A0$N%b%8%e!<%k(B stb_solver $BCf$K(B expand/3 $B$H(B combine/2 $B$r(B
  $BDj5A$7$?%U%!%$%k$r:n@.$;$M$P$J$j$^$;$s!#Nc$($P(B MY_SOL.kl1 $B$r:n@.$7$?(B
  $B>l9g$K$O!"(Bstb $B$H%@%_!<$N(B stb_solver $B$r%m!<%I$7$?8e!"(BMY_SOL.kl1 $B$r%3%s(B
  $B%Q%$%k$7!"$3$l$i$r:F%j%s%/$7$F$+$i;H$&I,MW$,$"$j$^$9!#(B

5. $B;29MJ88%(B
-----------

[1] M. Furuichi, K. Taki, and N. Ichiyoshi
     ``A Multi-Level Load Balancing Scheme for OR-Parallel Exhaustive
       Search Programs on the Multi-PSI'' 
    In Proc. of the 2nd ACM SIGPLAN Symposium on Principles and
    Practice of Parallel Programming}, pp. 50-59, Mar. 1990


[2] $B8E;T>;0l(B, $BBmOBCK(B, $B;T5H?-9T(B. 
     ``$BAB7k9gJBNs7W;;5!>e$G$N(B OR$BJBNsLdBj$KE,$7$?(B $BF0E*Ii2YJ,;6J}<0$H(B
       $B$=$NI>2A(B''.
     In Proceedings of KL1 Programming Workshop '90, pages 1-9, 
     Mar.1990.

[3] $B8E;T>;0l(B, $BCfEg9n?M(B, $BCfEg9@(B,  $B;T5H?-9T(B. 
      ``$B%9%?%C%/J,3dF0E*Ii2YJ,;6J}<0$H%^%k%A(BPSI$B>e$G$NI>2A(B''.
     In Proceedings of KL1 Programming Workshop '91, pages 51-58, 
     Mar. 1990.

[4] $B8E;T(B $B>;0l(B
        PIMOS $BIi2Y%P%i%s%9%f!<%F%#%j%F%#%^%K%e%"%k(B
                $B%9%?%C%/J,3dF0E*Ii2Y%P%i%s%5!<(B (STB)
                $B%^%k%A%l%Y%kF0E*Ii2Y%P%i%s%5!<(B (MLB)        
     ICOT TM $BEPO?=`HwCf(B. 
     $BK\%G%#%l%/%H%jFb(B balman.ps $B$,K\(BTM$B$NF|K\8l%]%9%H%9%/%j%W%H%U%!%$%k(B 
     $B7A<0$G$9!#(B

