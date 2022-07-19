clang \
        --target=wasm32 \
        --no-standard-libraries \
        -Wl,--export-all \
        -Wl,--no-entry \
        -Wl,-allow-undefined-file wasm.syms \
        -o main.wasm\
        src/fontrender.c\
        src/main.c\
        -Iinclude
