export const forkPtyAndExecvp: (
  file: string,
  argv: readonly string[],
  onExit: () => void
) => {
  readonly fd: number
  readonly pid: number
}
