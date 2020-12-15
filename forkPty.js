const bindings = require('bindings')

const addon = bindings('forkPty.node')

const forkPty = () => {
  return addon.forkPty()
}

exports.forkPty = forkPty
