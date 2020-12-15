const { forkPty } = require('./forkPty.js')
const { ReadStream } = require('tty')
const { performance } = require('perf_hooks')

for (let i = 0; i < 5; i++) {
  const s = performance.now()
  const fd = forkPty()
  const e = performance.now()
  console.log({ spawn: e - s })

  const readStream = new ReadStream(fd)
  let i = 0
  readStream.on('data', (data) => {
    if (++i == 2) {
      const e2 = performance.now()
      console.log({ data: e2 - s })
    }
    console.log({ data: data.toString() })
  })
}

setTimeout(() => {
  process.exit(0)
}, 3000)
