cd CGES
if [ -d ./embree3 ]; then
    echo "[CGES] Embree dependencies already installed!"
else
    echo "[CGES] Download embree dependencies..."
    wget https://github.com/embree/embree/releases/download/v3.6.1/embree-3.6.1.x86_64.linux.tar.gz
    echo "[CGES] Install embree dependencies..."
    tar -xvzf embree-3.6.1.x86_64.linux.tar.gz
    mv embree-3.6.1.x86_64.linux embree3
    ./embree3/embree-vars.sh
fi

if [ -d ./FindGLFW_Test ]; then
    echo "[CGES] GLFW .cmake files already installed!"
else
    echo "[CGES] Download GLFW .cmake files..."
    git clone git@github.com:benikabocha/FindGLFW_Test.git
fi

if [ -d ./bin ]; then
    echo "[CGES] cges binary already exists!"
else
    echo "[CGES] Build cges binaries..."
    mkdir bin
    cmake -DCMAKE_BUILD_TYPE=Release .
    make
fi
cd ..