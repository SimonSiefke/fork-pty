const { forkPty } = require('./forkPty.js')

test('forkPty', () => {
  const fileDescriptor = forkPty()
  expect(fileDescriptor).toBeGreaterThan(0)
})
