const BACKGROUND_COLOR = '#ddd';

/**
 * @type {SVGSVGElement}
 * */
let svg;
/**
 * @type {HTMLCanvasElement}
 * */
let canvas;

/**
 * @type {number[]}
 * */
let held_keys = [];

const env = {
  makeWindow: (width, height) => {
    canvas = document.createElement('canvas');
    document.querySelector('#container').prepend(canvas);

    canvas.width = width;
    canvas.height = height;

    const cxt = canvas.getContext('2d');
    cxt.fillStyle = BACKGROUND_COLOR;
    cxt.fillRect(0, 0, canvas.width, canvas.height);
  },
  clearWindow: () => {
    const cxt = canvas.getContext('2d');
    cxt.fillStyle = BACKGROUND_COLOR;
    cxt.fillRect(0, 0, canvas.width, canvas.height);
  },
  drawRect: (x, y, width, height, r, g, b) => {
    const cxt = canvas.getContext('2d');
    cxt.fillStyle = `rgb(${r}, ${g}, ${b})`;
    cxt.fillRect(x, y, width, height);
  },
  getHeldKey: () => held_keys[0] ?? -1,
  print: console.log,
  rand: Math.random,
  roundFloat: Math.round,
};

WebAssembly.instantiateStreaming(fetch('main.wasm'), { env }).then(async ({ instance }) => {
  let fpsInterval, now, then, elapsed;

  function startAnimating(fps) {
    instance.exports.init();

    fpsInterval = 1000 / fps;
    then = Date.now();
    startTime = then;
    animate();
  }

  function animate() {
    requestAnimationFrame(animate);

    now = Date.now();
    elapsed = now - then;

    if (elapsed > fpsInterval) {
      then = now - (elapsed % fpsInterval);
      instance.exports.update();
    }
  }

  window.addEventListener('keydown', (e) => {
    // Prevent scrolling using arrow keys
    if (e.key === 'ArrowDown' || e.key === 'ArrowUp') {
      e.preventDefault();
    }
    held_keys = [e.keyCode, ...held_keys];
  });

  window.addEventListener('keyup', (e) => {
    e.preventDefault();
    held_keys = held_keys.filter((keyCode) => e.keyCode !== keyCode);
  });

  startAnimating(60);

}).catch((err) => {
  console.log('Error!', err);
})
