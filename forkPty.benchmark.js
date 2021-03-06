const { forkPtyAndExecvp } = require('./forkPty.js')
const { ReadStream } = require('tty')
const { performance } = require('perf_hooks')

const noop = () => {}

const test1 = async () => {
  const s = performance.now()
  for (let i = 0; i < 100; i++) {
    await new Promise((r) => {
      const { fd } = forkPtyAndExecvp('ls', ['-l'], noop)
      const readStream = new ReadStream(fd)
      let j = 0
      readStream.on('data', (data) => {
        readStream.destroy()
        r()
      })
    })
  }
  const e = performance.now()
  console.log({ 'test1: average': (e - s) / 100 })
}

// const test2 = async () => {
//   const s = performance.now()
//   for (let i = 0; i < 30; i++) {
//     await new Promise((r) => {
//       const socket = forkPtyAndExecvp2('bash', ['-i'])
//       let j = 0
//       socket.on('data', (data) => {
//         if (++j == 2) {
//           r()
//         }
//       })
//     })
//   }
//   const e = performance.now()
//   console.log({ 'test2: average': (e - s) / 30 })
// }

;(async () => {
  await test1()
  // await test2()
  await test1()
  // await test2()
  await test1()
  // await test2()
  process.exit(0)
})()
