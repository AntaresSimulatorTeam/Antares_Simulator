
# Copy the changelog
file(READ "src/changelog.mdown" changelog_content)
file(WRITE "src/distrib/changelog.txt" "${changelog_content}")
