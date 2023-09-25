
# Copy the changelog
file(READ "../docs/CHANGELOG.md" changelog_content)
file(WRITE "distrib/changelog.txt" "${changelog_content}")
