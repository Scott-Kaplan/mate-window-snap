# Contributing

Thank you for your interest in improving `mate-snap-wm`.

## Project Scope

Contributions should target Ubuntu MATE with the MATE desktop, Marco, and an
X11 session. Support for other Linux distributions, desktop environments,
window managers, or Wayland is outside the current project scope.

## Before Opening A Pull Request

Please open an issue before starting a change and discuss the proposed behavior
or fix there. This helps confirm that the change fits the project's scope and
avoids duplicated work.

After the approach has been discussed, open a pull request that links to the
issue and clearly describes the change.

## Building And Testing

Run the build from the repository's root directory:

```sh
make
```

For code changes, the build must complete successfully. Manually test the
affected keyboard behavior when the required Ubuntu MATE/X11 environment and
hardware are available. Contributors do not need a two-monitor setup to submit
unrelated changes or documentation updates.

Documentation-only changes do not require a build, but please check the
Markdown for accuracy and readability.

## Code Style

Follow the existing code style and patterns. Keep changes focused, avoid
unrelated reformatting, and update documentation when behavior or setup steps
change.

## Issues And Pull Requests

Use issues for bug reports, proposed changes, and questions. Include the Ubuntu
MATE version, whether the session is X11, monitor configuration when relevant,
steps to reproduce, and any terminal output that helps explain the problem.

Pull requests should contain a focused change, explain how it was tested, and
identify any limitations or platform-specific behavior.

## Code Of Conduct

Participation in this project is subject to the [Code of Conduct](CODE_OF_CONDUCT.md).

## License

By contributing to this project, you agree that your contributions will be
licensed under the MIT License.
