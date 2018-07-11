Name:           rte-antares
Version:        %%VERSION%%
Release:        1%{?dist}
Summary:        A New Tool for Adequacy Reporting of Eletric Systems
Vendor:         RTE
License:        RTE Antares License
Group:          Applications/Simulators
URL:            http:://www.dma.tools.eu/antares

Source0:        %%SOURCE%%
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  chrpath

Requires:       glibc >= 2.12, libuuid >= 2.17, openssl >= 1.0

%description
Under the impulsion given by the worldwide growth of innovative technologies
(wind power, solar power, DC interconnectors, smart grids initiatives) and
new organizations (markets enlargments, regulatory frameworks), the assessment
of both adequacy and economic performance of large interconnected power systems
is  getting more challenging than ever.

Antares is a sequential Monte Carlo simulator designed to assess throughout a
whole year the economic behavior of large power systems with a 1-hour time
resolution and in several hundreds of stochastic scenarios regarding load and
generation (outages, wind speed, rainfall, nebulosity, temperature).

%prep
%setup -q


%build


%clean
rm -rf $RPM_BUILD_ROOT




%changelog
