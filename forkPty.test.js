const { forkPtyAndExecvp } = require('./forkPty.js')
const { ReadStream } = require('tty')

test('spawn bash', () => {
  const { fd } = forkPtyAndExecvp('bash', ['bash', '-i'])
  expect(fd).toBeGreaterThan(0)
})

test('spawn ls', async () => {
  const { fd } = forkPtyAndExecvp('ls', ['ls', '.github'])
  const readStream = new ReadStream(fd)
  const data = await new Promise((resolve) => {
    readStream.on('data', (data) => {
      readStream.destroy()
      resolve(data.toString())
    })
  })

  expect(data).toMatchInlineSnapshot(`
    "workflows
    "
  `)
})
