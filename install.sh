#!/bin/bash

function exit_with_error(){
    if [ $1 != 0 ]; then # exit statusが正常でない
        exit $1
    fi
}

cd CGES
# ↓diffとって判定した方がよさそう
echo "[CGES] Log files about installing will be generated in CGES_HOME/install_log"
if [ ! -d ../install_log ]; then
    mkdir ../install_log
fi

if [ -d ./embree3 ]; then
    echo "[CGES] Embree dependencies already installed."
else
    echo "[CGES] Downloading embree dependencies..."
    CGES_LOG_PATH="../install_log/install_embree.log"
    if [ -f $CGES_LOG_PATH ]; then
        rm $CGES_LOG_PATH
    fi
    date >> $CGES_LOG_PATH
    echo  >> $CGES_LOG_PATH
    wget https://github.com/embree/embree/releases/download/v3.6.1/embree-3.6.1.x86_64.linux.tar.gz >> $CGES_LOG_PATH
    exit_with_error $?
    echo "[CGES] Downloading DONE!"
    echo "[CGES] Installing embree dependencies..."
    tar -xvzf embree-3.6.1.x86_64.linux.tar.gz >> $CGES_LOG_PATH
    exit_with_error $?
    rm embree-3.6.1.x86_64.linux.tar.gz
    mv embree-3.6.1.x86_64.linux embree3
    source embree3/embree-vars.sh
    exit_with_error $?
    echo "[CGES] Installing DONE!"
fi

if [ -d ./FindGLFW_Test ]; then
    echo "[CGES] .cmake files for GLFW already installed."
else
    CGES_LOG_PATH=../install_log/install_findglfw.log
    if [ -f $CGES_LOG_PATH ]; then
        rm $CGES_LOG_PATH
    fi
    echo "[CGES] Downloading .cmake files for GLFW..."
    date >> $CGES_LOG_PATH
    echo  >> $CGES_LOG_PATH
    #git clone git@github.com:benikabocha/FindGLFW_Test.git >> $CGES_LOG_PATH
    wget https://github.com/benikabocha/FindGLFW_Test/archive/master.zip >> $CGES_LOG_PATH    
    exit_with_error $?
    unzip master.zip
    exit_with_error $?
    mv FindGLFW_Test-master FindGLFW_Test
    rm master.zip
    echo "[CGES] Downloading DONE!"
fi

# ↓diffとって判定した方がよさそう
if [ -f ./bin/cges ]; then
    echo "[CGES] cges build already completed!"
else
    echo "[CGES] Building cges binaries..."
    CGES_LOG_PATH=../install_log/install_cges.log
    if [ -f $CGES_LOG_PATH ]; then
        rm $CGES_LOG_PATH
    fi
    date >> $CGES_LOG_PATH
    echo  >> $CGES_LOG_PATH
    # ↓diffとって判定した方がよさそう
    if [ ! -d ./build ]; then
        mkdir build
    fi
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release .. >> ../$CGES_LOG_PATH
    exit_with_error $?
    make >> ../$CGES_LOG_PATH
    exit_with_error $?
    cd ..
    echo "[CGES] Building DONE!"
fi
cd ..