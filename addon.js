import bindings from 'bindings'

const addon = bindings('addon.node')

export const forkPty = () => {
  const fileDescriptor = addon.add()
  return fileDescriptor
}
