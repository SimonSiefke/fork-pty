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

// TODO
test.skip('valid input', (done) => {
  const { fd } = forkPtyAndExecvp('ls', ['ls', '.github'])
  const readStream = new ReadStream(fd)
  readStream.on('data', (data) => {
    console.log({ data: data.toString() })
  })
  readStream.on('error', (error) => {
    if (error.code === 'EIO') {
      return
    }
    throw error
  })
  readStream.on('close', (had_error) => {
    expect(had_error).toBe(false)
    done()
  })
})

test('invalid input', (done) => {
  const { fd } = forkPtyAndExecvp('non-existent-command', [])
  const readStream = new ReadStream(fd)
  readStream.on('data', () => {})
  readStream.on('error', (error) => {
    if (error.code === 'EIO') {
      return
    }
    throw error
  })
  readStream.on('close', (had_error) => {
    expect(had_error).toBe(true)
    done()
  })
})
