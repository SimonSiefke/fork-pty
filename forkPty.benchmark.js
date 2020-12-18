const {
  forkPtyAndExeclp,
  forkPtyAndExeclpe,
  forkPtyAndExecvp,
} = require('./forkPty.js')
const { ReadStream } = require('tty')
const { performance } = require('perf_hooks')

;(async () => {
  let total = 0
  for (let i = 0; i < 30; i++) {
    await new Promise((r) => {
      const s = performance.now()
      const fd = forkPtyAndExecvp('bash', ['-i'])
      const e = performance.now()
      console.log({ spawn: e - s })

      const readStream = new ReadStream(fd)
      let j = 0
      readStream.on('data', (data) => {
        if (++j == 2) {
          const e2 = performance.now()
          total += e2 - s
          console.log({ data: e2 - s })
          r()
        }
      })
    })
    console.log('\n\n')
  }
  console.log({ average: total / 30 })
  process.exit(0)
})()
