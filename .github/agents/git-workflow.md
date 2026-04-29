# Git Workflow

## Branch Naming

CI runs only for these branch name patterns:

- `feature/*` or `features/*`
- `fix/*`
- `release/*`
- `issue-*`
- `doc/*`
- `tests/`

Other patterns will not trigger CI.

## Git Hooks

Enable pre-commit hooks:

```bash
git config core.hooksPath .githooks
```

## Pull Request Guidelines

- **Title:** Must be self-explanatory (becomes commit title)
- **Description:** Include:
    - Motivation (why)
    - Method (how)
    - Proportional to PR complexity
- **Draft:** Set PR to draft if not ready for review
- **Documentation:** Update docs/ if changing documented features
