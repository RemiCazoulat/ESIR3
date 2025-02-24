@echo xcopy /Ding all necessary DLLs...
@IF "%1" == "" GOTO error 
@xcopy /D %AnimRenduDep%\lib\release\*.dll %1
@echo OK
@GOTO end

:error
@echo "ERROR: You need to provide a destination path"

:end
