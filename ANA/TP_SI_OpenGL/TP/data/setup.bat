@setx AnimRendu_DATA "%CD%"
@if errorlevel 1 @c:\windows\system32\setx AnimRendu_DATA "%CD%"
@if errorlevel 1 (echo "Critical error: unable to configure environment...") else echo "Configuration done, variable AnimRendu_DATA defined."
@pause
