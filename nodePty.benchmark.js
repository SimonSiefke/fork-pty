const { spawn } = require('node-pty')
const { performance } = require('perf_hooks')

const noop = () => {}

const test1 = async () => {
  const s = performance.now()
  for (let i = 0; i < 100; i++) {
    await new Promise((r) => {
      const pty = spawn('ls', ['ls', '-l'], {})
      pty.onData((data) => {
        pty.kill()
        r()
      })
    })
  }
  const e = performance.now()
  console.log({ 'test1: average': (e - s) / 100 })
}

;(async () => {
  await test1()
  // await test2()
  await test1()
  // await test2()
  await test1()
  // await test2()
  process.exit(0)
})()
