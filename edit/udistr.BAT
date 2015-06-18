if exist smain.wzd goto err
copy ..\..\edit\smain.*
copy ..\..\edit\secure.*
copy ..\..\edit\menu.*
copy ..\..\edit\makesec
copy ..\..\edit\32ksec.*
copy ..\..\edit\randhbit.*
copy ..\..\edit\ozlink48sec.*
copy ..\..\edit\secvars.*
goto end
:err
echo smain.wzd exists!
:end
