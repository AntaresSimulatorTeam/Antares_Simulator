<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
<assemblyIdentity
	name="com.rte-france.antares" type="win32"
	version="@ANTARES_VERSION_HI@.@ANTARES_VERSION_LO@.@ANTARES_VERSION_REVISION@.0"
	processorArchitecture="@ANTARES_MANIFEST_ARCH@"
/>
<description>Antares, v@ANTARES_VERSION_HI@.@ANTARES_VERSION_LO@.@ANTARES_VERSION_REVISION@</description>
<trustInfo xmlns="urn:schemas-microsoft-com:asm.v2">
	<security>
	<requestedPrivileges xmlns="urn:schemas-microsoft-com:asm.v3">
	<!-- UAC Manifest Options
	If you want to change the Windows User Account Control level replace the
	requestedExecutionLevel node with one of the following.
	<requestedExecutionLevel  level="asInvoker" uiAccess="false" />
	<requestedExecutionLevel  level="requireAdministrator" uiAccess="false" />
	<requestedExecutionLevel  level="highestAvailable" uiAccess="false" />

	If you want to utilize File and Registry Virtualization for backward
	compatibility then delete the requestedExecutionLevel node.
	-->
	<requestedExecutionLevel level="asInvoker" uiAccess="false" />
	</requestedPrivileges>
	</security>
</trustInfo>
<dependency>
	<dependentAssembly>
		<assemblyIdentity
			name="Microsoft.Windows.Common-Controls" version="6.0.0.0"
			type="win32"
			processorArchitecture="@ANTARES_MANIFEST_ARCH@"
			publicKeyToken="6595b64144ccf1df"
			language="*"
		/>
	</dependentAssembly>
</dependency>
</assembly>
