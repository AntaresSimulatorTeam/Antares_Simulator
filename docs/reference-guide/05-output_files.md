# Output files

The general file organization is the same for Economy and Adequacy simulations.

Economy and Adequacy results may be displayed in the GUI ( "Output" in main menu)

**Economy:**

| OUTPUT/Simu id/economy/mc-all/   |               |                 |                                       |
|----------------------------------|---------------|-----------------|---------------------------------------|
|                                  |/grid/...         |                 | contains a summary file "digest.txt"  |
|                                  |/areas/name/...|                 | contains area-related results         |
|                                  |/links / name/...|               | contains interconnection-related results |
|                                  |/mc-ind /year_number|  | |
|                                  |                    |/areas/name/...| contains area-related results|
|                                  |                    |/links/name/...| contains interconnection-related results|

_("mc-all" files contain synthetic results over all years, "year-number" files contain results for a single year)_
_The variables present in each file are detailed in the following sections._
_In "Economy" simulations, all variables have a techno-economic meaning._

**Adequacy:**

| OUTPUT/Simu id/adequacy/mc-all/   |               |                 |                                       |
|----------------------------------|---------------|-----------------|---------------------------------------|
|                                  |/grid/...         |                 | contains a summary file "digest.txt"  |
|                                  |/areas/name/...|                 | contains area-related results         |
|                                  |/links / name/...|               | contains interconnection-related results |
|                                  |/mc-ind /year_number|  | |
|                                  |                    |/areas/name/...| contains area-related results|
|                                  |                    |/links/name/...| contains interconnection-related results|

_("mc-all" files contain synthetic results over all years, "year-number" files contain results for a single year)_
_The variables present in each file bear exactly the same name as in Economy simulations but do not have the same values._
_The only variables that have a techno-economic meaning are the "Adequacy" indicators (unsupplied energy,LOLD,LOLP)_

**IMPORTANT** Adequacy and Economy files look the same but their content are specific

In "Economy" and "Adequacy" simulations, the optimization ignores the "primary" and "strategic" reserves (however, it may include the [other] spinning and day-ahead reserves, depending on the settings made in "optimization preferences").

In "Adequacy" simulations, all dispatchable thermal units are given the "must-run" status (hence, they will generate at Pmax, regardless of the demand). As a consequence the only variables that are actually meaningful are the adequacy indicators (unsupplied energy, LOLD,LOLP), that may depend on assumptions made regarding the economic values of Unsupplied and spilled energies, and on hurdle costs on interconnections.
In the specific case where binding constraints are present in the study, **all thermal clusters will consequently be de-activated from the binding constraints**. This can lead to incorrect adequacy indicators in Antares studies containing binding constraints in "Adequacy" simulations.

As a consequence, both "Adequacy" and "Economy" simulations yield the same values for the adequacy indicators under the following conditions: if hurdle costs on interconnections are higher than the difference between the maximum VOLL and the minimum VOLL assigned to the different areas of the system, and if no binding constraint is altered due to the fact that they contain clusters in must-run.

The files and their content are hereafter described.

## Economy and Adequacy, area results [^11]

**15** files resulting from the combination of the following attributes:
**[values | id | details] X [hourly | daily | weekly | monthly | annual]**

- The second attribute defines the time span over which the results are assessed: hourly detail, daily bundle, weekly bundle, monthly bundle, annual bundle.

- The first attribute defines the nature of the results presented in the file :

**Values** Values of different variables (price, load, overall generation issued from coal, etc.), the list of which is common to all areas of the interconnected system. Files of type "values" have therefore the same size for all areas.
These results appear under the label "general values" in the output GUI.

**details** Values regarding the different dispatchable thermal generating plants of each area (e.g. "older 300 MW coal from the south coast"). The sizes of these files differ from one area to another.
These results appear under the label "thermal plants" in the output GUI.

**id** Identifier (number) of the Monte-Carlo years for which were observed the extreme values of the different variables presented in the « values » files
These results appear under the label "record years" in the output GUI

The area files that belong to the « values » class display **122** fields corresponding to the expectation, standard deviation, minimal and maximal values of the variables whose list is given hereafter.

| variables | description |
|-----------|-------------|
| OV.COST | Overall cost = operating cost + unsupplied cost+ spilled cost+ hydro cost |
| OP.COST | Operating cost = Proportional costs + Non- proportional costs |
| MRG. PRICE | LMP : overall economic effect of a local 1MW load increase |
| CO2 EMIS. | Amount of CO2 emitted by all dispatchable thermal plants |
| NH3, SO2,... EMIS. | Amount emitted by all dispatchable thermal plants for other pollutants than CO2: SO2, NH3, NOX, PM2\_5, PM5, PM10, NMVOC, OP1, OP2, OP3, OP4, OP5 EMIS. |
| BALANCE | Overall Import/export balance of the area (positive value : export) |
| ROW BAL | Import/export with areas outside the modeled system (positive value: import) [^12] |
| PSP | User-defined settings for pumping and subsequent generating |
| MISC. NDG | Miscellaneous non dispatchable generation |
| LOAD | Demand (including DSM potential if relevant) |
| H.ROR | Hydro generation, Run-of-river share |
| WIND | Wind generation (only when using aggregated _Renewable generation modeling_)|
| SOLAR | Solar generation (thermal and PV) (only when using aggregated _Renewable generation modeling_)|
| NUCLEAR | Overall generation of nuclear clusters |
| LIGNITE | Overall generation of dispatchable thermal clusters burning brown coal |
| COAL | Overall generation of dispatchable thermal clusters burning hard coal |
| GAS | Overall generation of dispatchable thermal clusters burning gas |
| OIL | Overall generation of dispatchable thermal clusters using petroleum products |
| MIX.FUEL | Overall gen. of disp. thermal clusters using a mix of the previous fuels |
| MISC.DTG | Overall gen. of disp. thermal clusters using other fuels |
| MISC.DTG 2 | Overall gen. of disp. thermal clusters using other fuels |
| MISC.DTG 3 | Overall gen. of disp. thermal clusters using other fuels |
| MISC.DTG 4 | Overall gen. of disp. thermal clusters using other fuels |
| WIND OFFSHORE | Wind Offshore generation (only when using clustered _Renewable generation modeling_) |
| WIND ONSHORE | Wind Onshore generation (only when using clustered _Renewable generation modeling_) |
| SOLAR CONCRT. | Concentrated Solar Thermal generation (only when using clustered _Renewable generation modeling_) |
| SOLAR PV | Solar Photovoltaic generation (only when using clustered _Renewable generation modeling_) |
| SOLAR ROOFT | Rooftop Solar generation (only when using clustered _Renewable generation modeling_) |
| RENW. 1 | Overall generation of other Renewable clusters (only when using clustered _Renewable generation modeling_) |
| RENW. 2 | Overall generation of other Renewable clusters (only when using clustered _Renewable generation modeling_) |
| RENW. 3 | Overall generation of other Renewable clusters (only when using clustered _Renewable generation modeling_) |
| RENW. 4 | Overall generation of other Renewable clusters (only when using clustered _Renewable generation modeling_) |
| H.STOR | Power generated from energy storage units (typically: Hydro reservoir) |
| H.PUMP | Power absorbed by energy storage units (typically: PSP pumps consumption) |
| H.LEV | Energy level remaining in storage units (percentage of reservoir size) |
| H.INFL | External input to the energy storage units (typically: natural inflows) |
| H.OVFL | Wasted natural inflow overflowing from an already full energy storage unit |
| H.VAL | Marginal value of stored energy (typically: shadow water value) |
| H.COST | Expenses /Income brought by energy storage actions (H.STOR,H.PUMP) |
| UNSP. ENRG | Unsupplied energy: adequacy indicator (Expected Energy Not Served–EENS) |
| SPIL. ENRG | Spilled energy (energy produced that cannot be used and has to be wasted) |
| LOLD | Loss of load duration: adequacy indicator (length of shortfalls) |
| LOLP | Loss of Load probability: adequacy indicator (probability of at least one hour of shortfall within the considered period, without normalization by the duration of the considered period) |
| AVL. DTG | Available dispatchable thermal generation (sum of av. power over all plants) |
| DTG. MRG | Disp. Ther. Gen. (AVL DTG – sum of all dispatched thermal generation) |
| MAX. MRG | Maximum margin: operational margin obtained if the hydro storage energy of the week were used to maximise margins instead of minimizing costs |
| DENS | Domestic Energy Not Supplied: the difference between the local production capabilities of an area and its local load[^adqp] |
| LMR. VIOL | Local Matching Rule Violation after the Antares Simulation as defined by the adequacy patch[^adqp] |
| SPIL. ENRG. CSR | Spilled Energy after the Curtailment Sharing Rule step of the dequacy patch[^adqp] |
| NP COST | Non-proportional costs of the dispatchable plants (start-up and fixed costs) |
| NODU | Number of Dispatched Units [^13] |
| Profit | Net profit of the cluster in euros ((MRG. PRICE - marginal cost of the cluster) * (dispatchable production of the cluster)[^15]  |

Note: The net profit is computed on full precision values for MRG. PRICE. The user may obtain slightly different results applying the given formula because MRG. PRICE values are rounded to 10^-2.

## Economy and Adequacy, interconnection results [^14]
**10** files resulting from the combination of the following attributes:
**[values | id] X [hourly | daily | weekly | monthly | annual]**

- The second attribute defines the period of time over which the results are assessed: hourly detail, daily bundle, weekly bundle, monthly bundle, annual bundle.
- The first attribute defines the nature of the results presented in the file.


**values** values of different variables (flow, congestion rent) the list of which is common to all interconnections. The files of type "values" have therefore the same size everywhere
These results appear under the label "general values" in the output GUI.

**id** identifier (number) of the Monte-Carlo years for which were observed the extreme values of the different variables presented in the « values » files.
These results appear under the label "record years" in the output GUI.


The area files that belong to the « values » class display  **28**  fields corresponding to the expectation, standard deviation, minimal and maximal values of the variables whose list is given hereafter.

| variables | description |
|-----------|-------------|
| FLOW LIN. | Flow (signed + from upstream to downstream) assessed by the linear optimization. These flows follow Kirchhoff's law only if these laws have been explicitly enforced by the means of suitable binding constraints |
| UCAP | Used capacity: absolute value of FLOW LIN. This indicator may be of interest to differentiate the behavior of interconnectors showing low average flows: in some cases this may indicate that the line is little used, while in others this may be the outcome of high symmetric flows |
| LOOP FLOW | Flow circulating through the grid when all areas have a zero import/export balance. This flow, to be put down to the simplification of the real grid, is not subject to hurdle costs in the course of the optimization |
| FLOW QUAD. | Flow computed anew, starting from the linear optimum, by minimizing a quadratic function equivalent to an amount of Joule losses, while staying within the transmission capacity limits. This calculation uses for this purpose the impedances found in the "Links" Input data. If congestions occur on the grid, these results are not equivalent to those of a DC load flow|
| CONG. FEE ALG | Algebraic congestion rent = linear flow \* (downstream price – upstream price) |
| CONG. FEE ABS | Absolute congestion rent = linear flow\* abs(downstream price–upstream price) |
| MARG. COST | Decrease of the system's overall cost that would be brought by the optimal use of an additional 1 MW transmission capacity (in both directions) |
| CONG PROB + | Up&gt;Dwn Congestion probability = (NC+) / (total number of MC years) with: <br/> NC+ = number of years during which the interconnection was congested in the Up&gt;Dwn way for **any** length of time within the time frame relevant with the file|
| CONG PROB - | Dwn&gt;Up Congestion probability = (NC-) / (total number of MC years) with: <br/> NC- = number of years during which the interconnection was congested in the Dwn&gt;Up way for **any** length of time within the time frame relevant with the file|
| HURD. COST | Contribution of the flows to the overall economic function through the "hurdles costs" component. For each hour: <br/>`if (FLOW.LIN –LOOP FLOW) > 0 ` <br/> `HURD. COST = (hourly direct hurdle cost) * (FLOW LIN.)` <br/> `else HURD.COST = (hourly indirect hurdle cost) * (-1) * (FLOW LIN.)`|

## Economy and Adequacy, other results

Depending on the options chosen in the main simulation window, the output folders may also include either, both or none of the following sections:

| OUTPUT/Simu id/ts-numbers/   |                    |                                |
|------------------------------|--------------------|--------------------------------|
|                              |/Load               | /area names/...                |
|                              |/Thermal            | /area names/...                |
|                              |/Hydro              | /area names/...                |
|                              |/Wind[^agg]         | /area names/...                |
|                              |/Solar[^agg]        | /area names/...                |
|                              |/Renewables[^ren]   | /area names/...                |
|                              |/NTC                | /area names/...                |

These files contain, for each kind of time-series, the number drawn (randomly or not) in each Monte-Carlo year (files are present if "output profile / MC scenarios" was set to "true").

| OUTPUT/Simu id/ts-generator/ |                    |                                |
|------------------------------|--------------------|--------------------------------|
|                              |/Load               | /batch number/area names/...   |
|                              |/Hydro              | /batch number/area names/...   |
|                              |/Wind[^agg]         | /batch number/area names/...   |
|                              |/Solar[^agg]        | /batch number/area names/...   |


These files contain, for each kind of Antares-generated time-series, copies of the whole set of time-series generated. Batch numbers depend on the values set for the "refresh span" parameters of the stochastic generators (files are present if "store in output" was set to "true").

## Miscellaneous

Alike Input data, output results can be filtered so as to include only items that are associated with Areas and Links defined as "visible" in the current map. In addition, the output filtering dialog box makes it possible to filter according to two special categories (**Districts** and **Unknown**) that are not related to standard maps:

- **Districts** displays only results obtained for spatial aggregates
- **Unknown** displays only results attached to Areas or Links that no longer exist in the Input dataset (i.e. study has changed since the last simulation)

[^11]: This description applies to both « MC synthesis » files and "Year-by-Year" files, with some simplifications in the latter case

[^12]: Value identical to that defined under the same name in the "Misc Gen" input section.

[^13]: NODU and NP Cost do not appear in "Adequacy" results since these variables are irrelevant in that context

[^adqp]: Please note that this output variable is only available in the economy mode, when the adequacy patch is activated (see [Adequacy Patch](14-adequacy-patch.md))

[^14]: This description applies to both « MC synthesis » files and "Year-by-Year" files, with some simplifications in the latter case

[^agg]: This output is only available if the parameter "renewable generation modelling" is set to "cluster" in the input of the simulation

[^ren]: This output is only available if the parameter "renewable generation modelling" is set to "aggregated" in the input of the simulation

[^15]: dispatchable production = power generation above min gen = (power generation) - (min gen modulation)*units*capacity
