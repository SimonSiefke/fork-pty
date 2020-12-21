const { forkPtyAndExecvp } = require('./forkPty.js')

test('forkPtyAndExecvp', () => {
  const fileDescriptor = forkPtyAndExecvp('bash', ['-i'])
  expect(fileDescriptor).toBeGreaterThan(0)
})
