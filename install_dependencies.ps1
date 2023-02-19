New-Item ./CGES/dependencies -ItemType Directory
Set-Content ./CGES/dependencies/.gitignore "/*"

Invoke-WebRequest -Uri https://github.com/embree/embree/releases/download/v3.6.1/embree-3.6.1.x64.vc14.windows.zip -OutFile ./CGES/dependencies/embree-3.6.1.x64.vc14.windows.zip
Expand-Archive -Path ./CGES/dependencies/embree-3.6.1.x64.vc14.windows.zip -DestinationPath ./CGES/dependencies/
Rename-Item ./CGES/dependencies/embree-3.6.1.x64.vc14.windows/ -newName embree-3.6.1
Remove-Item ./CGES/dependencies/embree-3.6.1.x64.vc14.windows.zip