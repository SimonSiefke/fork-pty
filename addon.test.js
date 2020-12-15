import { forkPty } from './addon.js'

test('forkPty', () => {
  const fileDescriptor = forkPty()
  expect(fileDescriptor).toBeGreaterThan(0)
})
