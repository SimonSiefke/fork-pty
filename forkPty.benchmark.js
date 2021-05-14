import { forkPtyAndExecvp } from './forkPty.js'

const test1 = async () => {
  const s = performance.now()
  for (let i = 0; i < 100; i++) {
    await new Promise((r) => {
      const { fd, socket } = forkPtyAndExecvp('ls', ['-l'])
      let j = 0
      socket.on('data', (data) => {
        socket.destroy()
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
