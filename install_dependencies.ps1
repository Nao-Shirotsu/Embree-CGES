# 依存ライブラリ等をwebから取ってくるスクリプト

# ライブラリを入れておくフォルダを作る。DLする依存ファイルたちはgitignoreしておく。
New-Item ./CGES/dependencies -ItemType Directory
Set-Content ./CGES/dependencies/.gitignore "/*"

# Embree v3.6.1
Invoke-WebRequest -Uri https://github.com/embree/embree/releases/download/v3.6.1/embree-3.6.1.x64.vc14.windows.zip -OutFile ./CGES/dependencies/embree-3.6.1.x64.vc14.windows.zip
Expand-Archive -Path ./CGES/dependencies/embree-3.6.1.x64.vc14.windows.zip -DestinationPath ./CGES/dependencies/
Rename-Item ./CGES/dependencies/embree-3.6.1.x64.vc14.windows/ -newName embree
Remove-Item ./CGES/dependencies/embree-3.6.1.x64.vc14.windows.zip

# glm 0.9.9.8
Invoke-WebRequest -Uri https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip -OutFile ./CGES/dependencies/glm-0.9.9.8.zip
Expand-Archive -Path ./CGES/dependencies/glm-0.9.9.8.zip -DestinationPath ./CGES/dependencies/
Remove-Item ./CGES/dependencies/glm-0.9.9.8.zip

# GLFW 3.3
Invoke-WebRequest -Uri https://github.com/glfw/glfw/releases/download/3.3/glfw-3.3.bin.WIN64.zip -OutFile ./CGES/dependencies/glfw-3.3.bin.WIN64.zip
Expand-Archive -Path ./CGES/dependencies/glfw-3.3.bin.WIN64.zip -DestinationPath ./CGES/dependencies/
Rename-Item ./CGES/dependencies/glfw-3.3.bin.WIN64 -newName glfw
Remove-Item ./CGES/dependencies/glfw-3.3.bin.WIN64.zip

# stb library
Invoke-WebRequest -Uri https://github.com/nothings/stb/archive/refs/heads/master.zip -OutFile ./CGES/dependencies/stb-master.zip
Expand-Archive -Path ./CGES/dependencies/stb-master.zip -DestinationPath ./CGES/dependencies/
Rename-Item ./CGES/dependencies/stb-master -newName stb
Remove-Item ./CGES/dependencies/stb-master.zip