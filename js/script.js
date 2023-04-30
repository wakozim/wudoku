import {printf_init} from "./printf.js";

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

export async function startSudoku(printf_env) {
    
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

    let paused = true;
    sec.addEventListener("mouseenter", () => paused = false);
    sec.addEventListener("mouseleave", () => paused = true);

    Object.assign(libm, printf_env);
    const ctx = app.getContext("2d");
    const w = await WebAssembly.instantiateStreaming(fetch('./wasm/wudoku.wasm'), {
        "env": makeEnvironment(libm),
    });
    
    printf_init(w.instance.exports.memory);
    const heap_base = w.instance.exports.__heap_base.value;
    const buffer = w.instance.exports.memory.buffer;
    
    w.instance.exports.init_game();

    document.addEventListener('keydown', (e) => {
        w.instance.exports.keydown(e.key.charCodeAt());
        w.instance.exports.render_field(heap_base);
        renderCanvas(app, ctx, heap_base, buffer); 
    });
     
    w.instance.exports.render_field(heap_base);
    renderCanvas(app, ctx, heap_base, buffer); 
}
