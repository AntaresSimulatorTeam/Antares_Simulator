# Time-series analysis and generation

## General

When ready-made time-series are not available or are too scarce for building the required number of Monte-Carlo annual simulation scenarios, Antares provides means to generate sets of stochastic time-series to use instead.

The different categories of time-series call for wholly different generation processes:

- For thermal power, the generator is based on the animation of a daily three-state Markov chain (available – planned outage – forced outage) attached to each plant.

- For Hydro-power, the generator works out monthly time-series of energies, based on the assumption that they can be modeled by Log Normal variables with known correlations through space and time. So as to keep the model simple, for an interconnected system made of N areas, the user defines, along with the N expectations and N standard deviations of the monthly energies, the N X N correlation matrix R(n,m) of the logs of the annual hydro energies between the areas n,m, and the N average auto-correlations r(k) between one month and the next in each area k. The correlation **C(n,i,m,j)** between the logs of hydro energies in area **n**, month **i** and area **m**, month **j** is taken to be

$$C(n,i,m,j) = R(n,m)*\sqrt{(r(n) \cdot r(m))^{|j-i|}}$$
 

This most simplified model asks for considerably fewer data than a comprehensive 12N X 12N time-space matrix. Note that if R is positive semi-definite but C is not, matrix C is automatically transformed into a fitting p.s.d matrix and the data generation keeps going on (however, the log report will show a warning message). If the primary matrix R is not p.s.d, data are considered as corrupted, the generation stops and a fatal error message will be displayed in the log report

- For Wind power, Solar power and Load, the required time-series are 8760-hour long and have to emulate as closely as possible the response of the system to variations of wind speed, sunshine and temperature. In all three cases, the rationale of the model is to offer the possibility to consider either the final variable to model (wind power output, solar power output, load) or an underlying intermediate variable (wind speed, nebulosity, deviation between load and the level expected in standard temperature conditions) as a stationary stochastic process, with given marginal laws, given auto-correlation functions and given spatial correlations (eventually, the values of the final variables and those of the core stationary process are tied by diurnal/seasonal rhythms and scaling functions).

The identification of all relevant parameters can be made outside Antares by any appropriate means but can also be made automatically by the time-series analyzer, which is then to be fed with the largest available set of historical time-series. Note however that, using the time-series analyzer, one has to consider whether the time-series at hand are statistically meaningful or whether they need some pre-processing (for instance, if wind power time-series are gathered for a period within which the fleet has been expanded, the time-series to analyze should be expressed in % of installed power rather than in MW. For Solar power, the relevant variable to model as a stationary stochastic process is probably not the raw output of solar power but more likely a meteorological indicator related to the sky clarity (for instance , time-series of nebulosity expressed on a 0-100 scale may be used).

Once generated by appropriate algorithms, the values of the stationary processes are turned into final values by using a number of parameters that put back in the series the diurnal and seasonal patterns that may have been observed in the course of the historical data analysis and that were temporarily removed to identify the core stationary processes.


## Time-series generation (load, wind, solar): principles

For the generation of wind, solar and load time-series, Antares gives access to different marginal laws and autocorrelation functions presented hereafter. Note that wind speed modeling should usually be based upon a Weibull modeling, while almost all other situations are likely to be best modeled by Beta variables.

The stationary processes are defined at a monthly scale. For each month, there are:

- Four parameters for the definition of the marginal law

TS Gen. Parameters : $\alpha$, $\beta$, $\gamma$ and $\delta$

| **Law** | $\alpha$ | $\beta$ | $\gamma$ | $\delta$ | **Expectation** | **Variance** |
|---------|:----------:|:---------:|:----------:|:----------:|:---------------:|:------------:|
| Uniform | N/A | N/A | $< \delta$ | $> \gamma$ | ${(\delta - \gamma)\over 2}$ | ${(\delta - \gamma)^2\over 12}$ |
| Beta | &gt;0 | &gt;0 | $< \delta$ | $> \gamma$ | $\gamma + {\alpha(\delta - \gamma)\over (\alpha + \beta)}$ | $\alpha\beta(\delta - \gamma)^2\over (\alpha + \beta + 1)(\alpha + \beta)^2$ |
| Normal | Any | &gt;0 | N/A | N/A | $\alpha$ | $\beta^2$ |
| Weibull | &gt;=1 <br/> &lt;50 | &gt;0 | N/A | N/A | $\beta \Gamma (1 + {1\over\alpha})$ | $\beta^2[\Gamma(1+{2\over \alpha}) - \Gamma (1 + {1\over \alpha})^2]$ |
| Gamma | &gt;=1 <br/> &lt;50 | &gt;0 | N/A | N/A | $\alpha * \beta$ | $\alpha * \beta^2$ |

_Uniform: uniform defined on ($\gamma$, $\delta$)._  
_Beta: Beta ($\alpha$, $\beta$) defined on ($\gamma$, $\delta$)._  
_Normal: expectation $\alpha$, standard deviation $\beta$._  
_Weibull: shape $\alpha$, scale $\beta$, defined on (0,+$\infty$)._  
_Gamma: shape $\alpha$, scale $\beta$, defined on (0, +$\infty$)._

In the expressions of expectation and variance, $\Gamma(x)$ is the standard Euler Function.


- Two parameters for the definition of the autocorrelation function

**TS Gen. Parameters : $\theta$ and $\mu$**

| **Law** | $\theta$ | $\mu$ | **Corr(Xt, Xt+h)** |
|-----|----|-----|--------|
| Pure exponential decay | $\theta > 0$ | $\mu = 1$ | $e^{-\theta h}$ |
| Smoothed exponential decay [^decay] | $\theta > 0$ | $ 1 < \mu < 24$ | $\Phi(\theta, \mu, h)$ |

$$\Phi(\theta, \mu, h)\ =\ {1\over A}\ *\ \sum_{i=0, \mu}{\ \sum_{j=h, h+\mu}{e^{-\theta|j-i|}}}$$

**with** 

$$A=\mu + 2 \sum_{i=1, \mu; j=1, \mu; j > i}{e^{-\theta(j-i)}}$$


## Time-series generation (load, wind, solar): GUI

The section of the GUI specific to the generation of wind, solar and load time-series comprises:

1. **Spatial correlation matrices that are located within the "spatial correlation" tab of each path "Wind|Solar|Load / &lt;area\_name&gt;"**

	This tab contains a workspace for the description of 12 monthly spatial correlation matrices $\Xi$ and one annual correlation matrix. For the stochastic generators to work properly, these matrices must meet the usual requirements (matrices must be p.s.d, symmetric, with all terms between -100 and +100, and a main diagonal made of 100s). If this is not the case, generators will emit an infeasibility diagnosis. Matrices can be either set up manually OR automatically filled out by the time-series analyzer (see next paragraph).
	
	Depending on the choices made in the main "simulation" window, the matrices used will be either the 12 monthly matrices or the annual matrix. Whether to use the first or the second option depends on the quality of the statistical data at hand: with high quality data (for instance, that derived from the analysis of a very large pool of historical data), use of monthly correlations is recommended because monthly differences between matrices have a physical meaning ; with less robust data (derived from a handful of historical data,…), use of the single annual correlation matrix should be preferred because it smooths out the numeric noise which impairs the monthly matrices.

2. **Four parameters and four subtabs that are located within the "local" tab of each path "Wind|Solar|Load / &lt;area\_name&gt;"**

**FOUR PARAMETERS**

- Capacity: This first parameter is used to scale up time-series generated on the basis of the ($\alpha$, $\beta$, $\gamma$, $\delta$, $\theta$, $\mu$) parameters described previously in the "principles" paragraph, together with coefficients characterizing the diurnal pattern (see below)

- Distribution: This second parameter gives the type of marginal distribution of the stationary stochastic processes to generate (Beta, Weibull, Normal, Gamma, Uniform)

- Translation: This third parameter has three possible values:

    - Do not use: parameter ignored
    - Add before scaling: A specific 8760-hour array is added to the time-series produced by the primary stochastic generator, BEFORE use of the conversion table (optional) followed by the final multiplication by the capacity factor
    - Add after scaling: A specific 8760-hour array is added to the time-series produced by the primary stochastic generator, AFTER use of the conversion table (optional) followed by the final multiplication by the capacity factor

- Conversion: This fourth parameter has two possible values:

    - Do not use: Any transfer function that may be described in the "conversion" subtab (see below) should not be used for the final stage of data elaboration (for instance, if the primary parameters describe the physics of wind speeds, the time-series eventually produced should remain wind speeds and not wind power).

    - Use: The time-series produced by the stochastic generators (wind speeds, for instance) are turned into other values (wind power) by using the transfer function described in the "conversion" subtab.

**FOUR SUBTABS**

- Subtab "Coefficients"
	A twelve-month table of values for the primary parameters $\alpha$, $\beta$, $\gamma$, $\delta$, $\theta$, $\mu$ <br/>
	This table may be either filled out manually or automatically (use of the time-series analyzer)

- Subtab "Translation"
	Contains an 8760-hour array T to add to the time-series generated, prior or after scaling. This array can be either filled out manually or by the time-series analyzer.

- Subtab "Daily profile"
	A 24\*12 table of hourly / monthly coefficients K(hm) that are used to modulate the values of the stationary stochastic process by which the actual process is approximated. This table can be either filled out manually or by the time-series analyzer.

- Subtab "Conversion"
	A table of 2 \* N values (with 1&lt;=N&lt;=50) that is used to turn the initial time-series produced by the generators (for instance, wind speeds) into final data (for instance, wind power). The transfer function (speed to power, etc.) is approximated by N discrete points whose abscises X(N) an ordinates Y(N) are given by the table.

## Time-series analysis (load, wind, solar)

The time-series analyzer module available in Antares is meant to identify the values that should be given to the parameters used in the time-series generators (load, solar power, wind power) so as to fit best historical time-series at hand.

**IMPORTANT: ** When the time-series analyzer is used, it automatically updates the parameters relevant to the analysis (for instance: analysis of "wind" time-series will overwrite all local and global "wind" parameters [correlation matrices] that may have been previously set manually).

The primary TS analyzer window shows two tabs:

**1. Tab "Time-series and areas"**

- Time-series (load, wind, solar): class of parameters to be assessed by the analyzer

- Browse: location of the historical time-series files. These are txt files in which 8760-hour time-series must be stored in adjacent columns separated by a tabulation

- For each area:

    - Activity status
        - yes: parameters will be assessed and updated by the analyzer
        - no: the area will be skipped (**local** parameters for the area will remain unchanged, however **spatial** correlation with other areas will be reset to zero)

    - Distribution
        - Type of distribution to fit (beta, normal, etc.)

    - Data
        - Raw: data to analyze are the actual historical time-series
        - Detrended: data to analyze are the time-series of the deviations to average (for instance: load time-series need to be analyzed in "detrended" mode while wind speeds can be analyzed in "raw" mode)

    - File to analyze
        - Name of the file that should contain historical time-series to analyze
    - Status
        - Ready (a file bearing the expected name was found)
        - Not found (no file found with the expected name)

**IMPORTANT:** To generate stochastic data similar to the historical data analyzed, generation parameters must be kept consistent with the results of the analysis, which means, in the generators:

- Keep the same:

    - Type of distribution
    - Values for $\alpha$, $\beta$, $\gamma$, $\delta$ and for the diurnal–seasonal pattern (table of 12 X 24 values)
    - Value for the "capacity" parameter (the analyzer automatically sets it to 1)

- Besides:
    - "Conversion" option must be set to "no"
    - "Translation" option must be set to "do not use "if data were analyzed as "raw" and to "add after scaling" or "add before scaling" if data were analyzed as "detrended" (both options give the same value in this case because the scaling is 1:1)

**2. Tab "Global settings"**

- Temporary folder: workspace that can be used for the analysis (cleaned after use)

- Analyzer settings

    - Short-term autocorrelation adjustment (%)
    - Long-term autocorrelation adjustment (%) <br/>
      These two parameters are used by Antares as targets for the fitting of $\theta$ and $\mu$ parameters. For instance, if the historical time-series autocorrelation function is such that Corr(T,T+ 18 hours)=90 % and Corr(T,T+60 hours)= 50%, and if the parameters in the analyzer are (ST = 90%,LT = 50%) , then it will search values of $\theta$ and $\mu$ matching the historical autocorr.function in two points(18 hours, 60 hours).

    - Trimming threshold (%) <br/>
      In the spatial correlation matrices, terms lower than the threshold will be replaced by zeroes

- Input data

    - Time-series per area (n) <br/>
      limits the analysis to the first n historical time-series at hand
    - Upper-bound (Max) <br/>
      In the analysis, all values above Max in the historical files will be replaced by Max
    - Lower-bound (Min) <br/>
      In the analysis, all values below Min in the historical files will be replaced by Min

**IMPORTANT:** For each month, time-series to analyze are assumed to represent a stationary stochastic signal modulated by 24 hourly shape-factors. All of these shape-factors are expected to be different from zero. If the signal is partly masked by sequences of zeroes (for instance, if solar power time-series are to be analyzed as such because time-series of nebulosity are not available), the analysis is possible but is subject to the following restrictions:

- **Use of the "detrended" mode in the first Tab is mandatory** _(use of the "raw" mode would produce wrong correlation matrices)_

- **Short- and Long- Term autocorrelation parameters in the second Tab must be identical and set to 99%** _(to ensure that auto-correlation be assessed for the shortest possible time lag, i.e. one hour)_

**NOTICE:** For the whole year, the analyzer delivers a table of 12x24 hourly shape-factors consistent with the 12 sets of parameters identified for the stationary stochastic processes. The content of the table depends on the mode of analysis chosen:

- **"raw"** analysis: for each month, the sum of the 24 hourly shape-factors is equal to 24 (i.e. each term is a modulation around the daily average).

- **"detrended"** analysis: for the whole year, hourly coefficients are expressed relatively to the annual hourly peak of the (zero-mean) signal absolute value. (i.e. all factors belong to the [0,1] interval)

## Time-series generation (thermal)

The thermal time-series generation will only be launched:
  - On thermal clusters that have the Generated TS parameter set to “Force generation”
  - And, when in the Simulation window, the Stochastic TS parameter for Thermal is set to "On”, on the thermal clusters that have the Generated TS parameter set to "Use global parameter".

The stochastic generator for time-series of available dispatchable power generation works, for each plant of each set (cluster), with the following parameters:

- The nominal plant capacity and a 8760-hour array of modulation coefficients to apply to it (default value: 1)

- A 365-day array of forced outages rates ("FOR", lies in [0,1] )

- A 365-day array of planned outages rates ("POR", lies in [0,1])

- A 365-day array of forced outages average durations ("FOD" in days, integer, lies in [1,365])

- A 365-day array of planned outages average durations ("POD" in days, integer,lies in [1,365])

- A 365-day array of planned outages minimum number (PO Min Nb) (integer, lies in [0, PO Max Nb])

- A 365-day array of planned outages maximum number (PO Max Nb) (integer, lies in [PO Min Nb, Nb of units in the cluster]

- Two parameters describing how forced outages durations may randomly deviate from their average value (law: uniform or geometric , volatility: lie in [0,1])

- Two parameters describing how planned outages durations may randomly deviate from their average value (law: uniform or geometric , volatility: lie in [0,1])

**1. Outage duration : meaning and modeling**

In the thermal time-series generator, the concept of outage duration (either forced or planned) is simple enough: for any given plant affected by such an event, it is the duration of a single outage, expressed in days.

The fact that 365 different values can be used to describe what may happen in the course of a year (for each kind of outages) means that the average outage duration may depend on the day the outage begins on. For instance, very short outages may be sometimes be planned on week-ends. Likewise, historical statistics can show that forced outages do not last the same average time in winter and summer, etc.

In complement to the average value of the duration D of outages beginning on a particular day, the time-series generator allows to set two parameters that describe how the actual outage durations may deviate from the calendar-related average value.

- The first parameter (law) can take either the value "uniform" or "geometric": <br/>
  <ins>Uniform</ins>: the actual outage duration will be randomly drawn (one draw per outage), according to a **uniform distribution** centred on the average value **D**. The width of the interval [min duration, max duration] will depend on the value of the second parameter (volatility). <br/>
  <ins>Geometric</ins>: the actual outage duration will be expressed as the sum of a fixed value F and a randomly drawn (one draw per outage) variable following a **geometric distribution** of expectation G, with **F+G=D**. The ratio of F to G will depend on the value of the second parameter (volatility).


- The second parameter (volatility) can take any value within [0,1]:

    - 0: The outage duration does not show any stochastic fluctuation at all. <br/>
      Therefore, regardless of the chosen distribution law: <br/>
      **<center>actual duration = D</center>**

    - 1: The variability of the actual outage duration is as high as the chosen distribution law makes it possible, which means respectively that:

        - If choice = "uniform": **1 &lt;= actual duration &lt;= 2D-1**
        - If choice = "geometric": **F = 0 and G = D** <br/>
          (which in turn implies 1 &lt;= actual duration &lt;= #4D)

    - 0&lt;V&lt;1: The variability of the actual outage duration is such that the ratio $\sigma / D$ of its standard deviation to its expectation has a value that depends on **V** , on **D** and on the chosen distribution law. More precisely:

        - If choice = "uniform": $\sigma / D = [{1/3}^{0.5}] \* V \* (D-1) / D $ <br/>
          and <br/>
          **Duration min = D (1-V) + V** <br/>
          **Duration max = D (1+V) - V**

        - If choice = "geometric": $\sigma / D = V \* [(D-1) / D]^{0.5}$ <br/>
          and <br/>
          **Duration min = F** <br/>
          **Duration max # 4D-3F** <br/>
          _with F = D – G_ <br/>
          _G = 2z /[(1+4z)^0.5 - 1]_ <br/>
          _z = (V^2) \* D \* (D-1)_

**NOTE:** _The calculation time required for the generation of time-series does not depend of the kind of chosen law but depends on the fact that the volatility is null or not (it is minimal for zero-volatility)._

**NOTE:** _A geometric law associated with a volatility parameter V yielding a characteristic parameter F (according to the previous formulas) will produce a distribution summarized by:_

- _63 % of values in the interval [F, D]_
- _23 % of values in the interval [D, 2D-F]_
- _12 % of values in the interval [2D-F, 4D-3F]_
- _2 % of values in the interval [4D-3F, infinite)_

**Remark:** _Antares is able to provide these options because it involves more than a simple Markov chain mechanism (intrinsically limited to : law = geometric, volatility = 1)_

**2. Outage rates: meaning and modeling**

The concept of outage rate is not always clearly distinguished from the notion of failure rate, to which it is closely related.

Outage rates OR represent the average **proportion** of time during which a plant is unavailable (for instance, OR = 5.2%).

Failure rates FR represent the average **number** of outages **starting** during a period of time of a given length (for instance, FR = 1.5 per year). If the time step is short enough (typically one day, which is the value used in Antares), the failure rates are always lower than 1 (for instance, FR = (1.5 / 365) per day).

When this condition is met and if the physical outage process can be modelled by a Poisson process, failure rates can be interpreted as probabilities.

In Antares the following relation between failure rates FR, outage rates OR and outage durations OD is used:

**<center>FR = OR / [OR+ OD \* ( 1 – OR )]</center>**

To determine whether a plant available on day D is still available on day D+1, the Antares stochastic generator therefore makes draws based on the failure rates equivalent to the data provided in the form of outage rates and outage durations.

Since two processes may be described in the GUI, consecutive draws are made for each process so as to determine whether:

- An outage of the first category begins (it will last for the specified duration)
- An outage of the second category begins (it will last for the specified duration)
- No outage occurs, the plant is still available on D+1

Whether to describe the "planned outage" process as a random process or not depends of the kind of data at hand, which is often related to the horizon of the studies to carry out: when actual overhauls plans are known, the PO rates can be set at 1 when the plant is deemed to be unavailable and to zero on the other days.

For long term studies in which only general patterns are known, season-, month- or week- modulated rates and duration may be used to describe the "planned" process as a stochastic one. Another possible use of the model is to incorporate the overhauls plans in the "nominal capacity modulation" array, and consider the stochastic "planned outage" processor as a simulator for a second modality of forced outage (longer or shorter than the main component)

**NOTE:** _Once the outage duration and outage rate are defined, the failure rate is completely determined. For the sake of clarity, the Antares GUI displays still another parameter often used in reliability analysis, which is the MTBF (Mean Time Between Failure). Relations between MTBF, FR and OR are:_

_**<center>FR= 1 / ( MTBF+1 ) OR = OD / ( MTBF+OD )</center>**_

**NOTE:** _When two stochastic processes of outages (forced and planned, or forced-type-1 and forced-type-2) are used, the overall resulting outage rate OOR is not equal to the sum of the two rates FOR and POR. Instead, the following relation holds:_

_**<center>OOR = ( FOR + POR – 2\*FOR\*POR ) / (1 - FOR\*POR)</center>**_

_The explanation of this formula lies in the definition of the different outages rates:_

_Over a long period of operation, FOR represents the ratio of the time spent in forced outages to the overall time not spent in planned outages._

_Likewise, POR represents the ratio of the time spent in planned outages to the overall time not spent in forced outages._

_OOR represents the ratio of the time spent in either forced or planned outages to the overall operation period._

_The period of operation can be broken down into three categories of hours:_

_F hours spent in forced outages_

_P hours spent in planned outages_

_A hours of availability_

_The following relations hold and explain the previous formula:_

_FOR = F/(A+F)_

_POR=P/(A+P)_

_OOR=(F+P)/(A+F+P)_

**3. Planned Outages Minimum and Maximum Numbers**

In the description given so far regarding how outages are modeled, no true difference was made between "forced" and "planned" outages, i.e. both relied on unconstrained random draws. This is satisfactory only if the process to model through the "planned" data is actually little constrained, or not at all.

In all other occurrences, it makes sense to define a general framework for the maintenance schedule. In Antares this is defined at the cluster scale by two specific 365-day arrays:

<center>PO Min Nb and PO Max Nb.</center>

These parameters are used by the time-series generator as constraints that **cannot be violated**, regardless of the raw outcome of regular random draws. To meet these constraints, the generator may have to anticipate or delay "planned" outages yielded by the primary random draws stage. If data regarding planned outage rates and planned outage Max and Min numbers are not consistent, the Max and Min Numbers take precedence.

_Examples (for simplicity'sake, they are described here with only one value instead of 365):_

_Cluster size = 100 PO rate =10% PO Min Nb=0 PO Max Nb= 100_

- _Actual number in [0,100], average = 10, wide fluctuations (unconstrained)_

_Cluster size = 100 PO rate =10% PO Min Nb=7 PO Max Nb= 11_

- _Actual number in [7,11], average = 10 (to remain within the bounds, some outages will be anticipated, while others will be delayed)_

_Cluster size = 100 PO rate =0% PO Min Nb=10 PO Max Nb= 10_

- _Actual number =10 (to remain within the bounds, outages are set up even if none come from random draws)_

## Time-series analysis (thermal)

The stochastic generator for time-series of available dispatchable power generation needs to be given assumptions regarding forced &amp; planned outages rates &amp; durations. Depending on the quality and quantity of statistics at hand, these estimates can be either described as "flat" (same constant values used from the beginning to the end of the year) or as more or less modulated signals, with the possibility of choosing different values for each day of the year.

Different ways can be considered to work out values for FOR,POR,FOD,POD from historical data regarding outages. For any (family of) plant(s) to study, notations have to be defined with respect to the "calendar accuracy" chosen by the user. For the sake of clarity, assume from now on that the user wants to assess weekly rates and durations, that is to say: describe the whole year with 52 values for rates and durations, for both forced and planned outages (within any given week, identical values will therefore be assigned to the seven days of the week).

With the following notations:

- D(w) = Overall cumulated statistical observation time available for week (w)  
	for instance, for w = 1= first week of January : D(w) = 3500 days coming from 10 years of observation of 50 identical plants

- Df(w) = Within D(w), overall time spent in forced outages, either beginning during week w or before (for instance , Df(1) = 163 days)

- Dp(w) = Within D(w), overall time spent in planned outages, either beginning during week w or before (for instance, Dp(1) = 22 days)

- Kf(w) = Number of forced outages beginning during week (w)  
	(for instance, Kf(1) = 26)

- Kp(w) = Number of planned outages beginning during week (w)  
	(for instance, Kp(1) = 3)

- FOT(w) = Overall cumulated time (expressed in days) spent in forced outages beginning during week (w) (for instance, FOT(1)= 260)  
	Note that if outages last more than one week FOT(w) necessarily includes days from weeks w+1, w+2,…

- POT(w) = Overall cumulated time (expressed in days) spent in planned outages beginning during week (w) (for instance, POT(1) = 84)  
	Note that if outages last more than one week POT(w) necessarily includes days from weeks w+1, w+2,…

The following formulas can be used :

**FOD (w) = FOT(w) / Kf(w)**

**POD (w) = POT(w) / Kp(w)**

**FOR(w) = FOD(w) / [FOD(w) + ( (D(w) - Dp(w)) / Kf(w))]**

**POR(w) = POD(w) /[POD(w) + ( (D(w) - Df(w))) / Kp(w))]**

_For the examples given above, the estimated parameters would therefore be :_

_FOD(1) = 10 (days)_

_POD(1) = 28 (days)_

_FOR(1) = 0.0695 # 7 %_

_POR(1) = 0.0245 # 2.5 %_

_These values should eventually (using the GUI or other means) be assigned to the first seven days of January._

## Time-series generation and analysis (hydro)

The stochastic hydro generator assesses monthly time-series of energies, based on the assumption that they can be modeled by Log Normal variables. The values generated are interpreted as monthly amounts of hydro energies generated (sum of Run of River – ROR – and hydro storage – HS) or as amounts of hydro inflows, depending on the modeling chosen for the area (straightforward estimate of energies generated or explicit management of reservoirs).

The historical data to work from depend on the kind of modeling chosen (statistics of monthly generation in the first case, or statistics of monthly inflows in the second case).

In both cases, assuming that a large number of historical time-series of energies are available, the rationale of the assessment of parameters is the following (from now on, "energies" mean either "ROR and HS energies generated" or "inflows to ROR and HS"),

1. For each area n, build up annual energy time-series **A(n)** by aggregation of the original monthly energy time-series **M(n)**. For each pair of areas (n,m) , assess the correlation **R(n,m)** between the random variables **Log(A(n))** and **Log(A(m))**. Expressed in percentage, matrix **R** should be used to fill out the "spatial correlation tab" of in the active window "hydro"

2. For each area n, build up two monthly time-series derived from the original array **M(n)**, by proceeding as follows. Assuming that **M(n)** has K elements (for instance, K= 180 if 15 years of statistics are available):

    - **M'(n)** = time-series of K-1 elements obtained by deleting the first element of the time-series Log(M(n))
    - **M''(n)** = time-series of K-1 elements obtained by deleting the last element of the time-series Log(M(n))

	Assess the correlation **IMC(n)** between the random variables **M'(n)** and **M''(n)**. This value (lying in [-1,1]) should be used to fill out the field "inter-monthly correlation value" of the "local data" tab in the "hydro" active window.

3. For each area n, build up 12 monthly energy time-series derived from the original array **M(n)** by extracting from **M(n)** the values related to each month of the year (**M1(n)**= time-series of energies in January,…, **M12(n)** = time-series of energies in December.)

	Assess the expectations and standard deviations of the 12 random variables **M1(n)** ,…, **M12(n)**. These values should be used to fill out the fields "expectation" and "std deviation" of the "local data" tab in the "hydro" active window.

	Aside from expectation and standard deviations, minimum and maximum bounds can be freely set on the monthly overall energies (ROR + HS). Whether to assess these bounds by examination of historical data or on the basis of other considerations depends on the context of the studies to carry out.

4. For each area n, extract from the 12 monthly overall energy time-series **M1(n) ,…, M12(n)** the contribution of the 12 monthly time-series of ROR energies **R1(n),…, R12(n)**.

	Assess the expectations of the 12 random variables **R1(n)/M1(n),…., R12(n)/M12(n)** . These values should be used to fill out the fields "ROR share" of the "local data" tab in the "hydro" active window.

[^decay]: Obtained by the generation of purely exponentially autocorrelated values (parameter $\theta$ ) followed by a moving average transformation (parameter $\mu$ ). $\theta$ and $\mu$ should be carefully chosen so as to accommodate at best the experimental data at hand. If meaningful historical data are available, this identification may be directly made using the Antares time-series analyzer.