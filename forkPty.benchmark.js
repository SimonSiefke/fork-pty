const { forkPtyAndExeclp } = require('./forkPty.js')
const { ReadStream } = require('tty')
const { performance } = require('perf_hooks')

;(async () => {
  for (let i = 0; i < 30; i++) {
    await new Promise((r) => {
      const s = performance.now()
      const fd = forkPtyAndExeclp('bash', '-i')
      const e = performance.now()
      console.log({ spawn: e - s })

      const readStream = new ReadStream(fd)
      let j = 0
      readStream.on('data', (data) => {
        console.log({ data: data.toString() })
        if (++j == 2) {
          const e2 = performance.now()
          console.log({ data: e2 - s })
          r()
        }
      })
    })
    console.log('\n\n')
  }
  process.exit(0)
})()
