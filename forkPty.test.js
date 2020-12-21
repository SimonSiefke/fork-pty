const { forkPtyAndExecvp } = require('./forkPty.js')
const { ReadStream } = require('tty')

test('spawn bash', () => {
  const fd = forkPtyAndExecvp('bash', ['bash', '-i'])
  expect(fd).toBeGreaterThan(0)
})

test('spawn ls', async () => {
  const fd = forkPtyAndExecvp('ls', ['ls'])
  const readStream = new ReadStream(fd)
  const data = await new Promise((resolve) => {
    readStream.on('data', (data) => {
      readStream.destroy()
      resolve(data.toString())
    })
  })

  expect(data).toMatchInlineSnapshot(`
    "binding.gyp  forkPty.benchmark.js  forkPty.js	    package.json
    build	     forkPty.cc		   forkPty.test.js  package-lock.json
    example      forkPty.d.ts	   node_modules
    "
  `)
})
