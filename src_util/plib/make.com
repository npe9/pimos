%(C)1992 Institute for New Generation Computer Technology
%	(Read COPYRIGHT for detailed information.)
% icpsi329::>sys>user>yashiro>PIMOS_TOOL>LIBRARY>make.com.6, 29-Jan-91 13:44:38, Edit by yashiro
compile("library").
compile(["atos","new_window","new_file","new_timer","new_string_io","error","long","version"]).
unload(['$plib$atos','$plib$window','$plib$file','$plib$timer','$plib$string_io','$plib$error','plib','$plib$long','$plib$version'],"plib").
change_macro(plib).
