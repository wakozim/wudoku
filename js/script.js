function cstrlen(mem, ptr) {
    let len = 0;
    while (mem[ptr] != 0) {
        len++;
        ptr++;
    }
    return len;
}

function cstr_by_ptr(mem_buffer, ptr) {
    const mem = new Uint8Array(mem_buffer);
    const len = cstrlen(mem, ptr);
    const bytes = new Uint8Array(mem_buffer, ptr, len);
    return new TextDecoder().decode(bytes);
}

function readCanvasFromMemory(memory_buffer, canvas_ptr)
{
    const canvas_memory = new Uint32Array(memory_buffer, canvas_ptr, CANVAS_SIZE);
    return {
        pixels: canvas_memory[CANVAS_PIXELS],
        width: canvas_memory[CANVAS_WIDTH],
        height: canvas_memory[CANVAS_HEIGHT],
        stride: canvas_memory[CANVAS_STRIDE],
    };
}

function renderCanvas(app, ctx, heap_base, buffer)
{
    const canvas = readCanvasFromMemory(buffer, heap_base);
            
    if (canvas.width != canvas.stride) {
        // TODO: maybe we can preallocate a Uint8ClampedArray on JavaScript side and just copy the canvas data there to bring width and stride to the same value?
        console.error(`Canvas width (${canvas.width}) is not equal to its stride (${canvas.stride}). Unfortunately we can't easily support that in a browser because ImageData simply does not accept stride. Welcome to 2022.`);
        return;
    }
    const image = new ImageData(new Uint8ClampedArray(buffer, canvas.pixels, canvas.width*canvas.height*4), canvas.width);
    app.width = canvas.width;
    app.height = canvas.height;
    ctx.putImageData(image, 0, 0);
}

function makeEnvironment(...envs) {
    return new Proxy(envs, {
        get(target, prop, receiver) {
            for (let env of envs) {
                if (env.hasOwnProperty(prop)) {
                    return env[prop];
                }
            }
            return (...args) => {console.error("NOT IMPLEMENTED: "+prop, args)}
        }
    });
}

function getRandomInt() {
  return Math.floor(Math.random() * 10000);
}

let libm = {
    "atan2f": Math.atan2,
    "cosf": Math.cos,
    "sinf": Math.sin,
    "sqrtf": Math.sqrt,
    "rand": getRandomInt, 
};

let iota = 0;
// TODO: nothing in this Canvas "declaration" states that iota's measure units are Uint32
// Which is not useful for all kinds of structures. A more general approach would be to use Uint8 as the measure units.
const CANVAS_PIXELS = iota++;
const CANVAS_WIDTH  = iota++;
const CANVAS_HEIGHT = iota++;
const CANVAS_STRIDE = iota++;
const CANVAS_SIZE   = iota++;


function getKeyPressed(w, key) {
    if (key == 'KeyW' || key == 'ArrowUp') {
        return 10;
    } else if (key == 'KeyS' || key == 'ArrowDown') {
        return 11; 
    } else if (key == 'KeyA' || key == 'ArrowLeft') {
        return 12; 
    } else if (key == 'KeyD' || key == 'ArrowRight') {
        return 13; 
    } else if (key.startsWith('Digit')) {
        return Number(key[5]); 
    } else if (key.startsWith('Numpad')) {
        return Number(key[6]); 
    }

}

function refreshCanvas() {
    const heap_base = w.instance.exports.__heap_base.value;
    w.instance.exports.render_game(heap_base);
    const app = document.getElementById(`app-sudoku`);
    const ctx = app.getContext("2d");
    const buffer = w.instance.exports.memory.buffer;
    renderCanvas(app, ctx, heap_base, buffer);
}

function changeDifficulty() {
    var difficulty = document.getElementById("difficulty").value;
    w.instance.exports.change_difficulty(Number(difficulty));
    refreshCanvas()
}

function changeColorscheme() {
    var colorscheme = document.getElementById("colorscheme").value;
    w.instance.exports.change_colorscheme(Number(colorscheme));
    refreshCanvas()
}

function restartGame() {
   w.instance.exports.reset_field();
   refreshCanvas()
}

async function startSudoku() {
   const app = document.getElementById(`app-sudoku`);
    if (app === null) {
        console.error(`Could not find element app-sudoku. Stop loading sudoku.`);
        return;
    }
    const sec = document.getElementById(`wudoku`);
    if (sec === null) {
        console.error(`Could not find element wudoku. Stop loading sudoku.`);
        return;
    }

    //Object.assign(libm, printf_env);
    const ctx = app.getContext("2d");
    w = await WebAssembly.instantiateStreaming(fetch('./wasm/wudoku.wasm'), {
        "env": makeEnvironment(libm),
    });
    
    //printf_init(w.instance.exports.memory);
    const heap_base = w.instance.exports.__heap_base.value;
    const buffer = w.instance.exports.memory.buffer;
    
    w.instance.exports.init_game();

    document.addEventListener('keydown', (e) => {
        w.instance.exports.keydown(getKeyPressed(w, e.code));
        if (e.code == 'KeyR') {
            w.instance.exports.reset_field();
        }
        w.instance.exports.render_game(heap_base);
        renderCanvas(app, ctx, heap_base, buffer); 
    });
     
    w.instance.exports.render_game(heap_base);
    renderCanvas(app, ctx, heap_base, buffer); 
}

var w = null;
startSudoku();
