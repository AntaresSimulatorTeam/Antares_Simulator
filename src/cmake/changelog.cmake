
# Copy the changelog
file(READ "changelog.mdown" changelog_content)
file(WRITE "distrib/changelog.txt" "${changelog_content}")
