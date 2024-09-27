
# Copy the changelog
file(READ "../docs/developer-guide/CHANGELOG.md" changelog_content)
file(WRITE "distrib/changelog.txt" "${changelog_content}")
