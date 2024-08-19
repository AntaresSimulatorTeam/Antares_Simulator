# Hydro heuristics

_**This section is under construction**_

## Seasonal hydro pre-allocation

Basically, the seasonal hydro pre-allocation process comprises two stages carried out two times
(first time: monthly scale; second time: daily scale).

- Stage 1: Definition of an allocation ideal modulation profile, which may be based (or not) on local and/or
  remote load profiles.
- Stage 2: Mitigation of the previous raw profile to obtain a feasible hydro ideal target,
  compatible as much as possible with reservoir rule curves.

The description given hereafter makes use of the following local notations,
not be confused with those of the document "optimization problem formulation"
(dedicated to the optimal hydro-thermal unit-commitment and dispatch problem):

- $Z$ Number of Areas (zones $z$) in the system
- $M_{zh}$ Hourly time-series of cumulated must-generation of all kinds for zone $z$
- $M_{zd}$ Daily time-series of cumulated must-generation of all kinds for zone $z$ (sum of $M_{zh}$)
- $M_{zm}$ Monthly time-series of cumulated must-generation of all kinds for zone $z$ (sum of $M_{zh}$)
- $M_{z.}$ Either $M_{zd}$ or $M_{zm}$, relevant time index "." is defined by the context
- $L_{z.}$ Time-series of "natural" load for zone $z$
- $A$ Inter-area hydro-allocation matrix (dimension_ $x Z$ ) $A_{uv}$ is a weight given to the load
  of area $u$ in the definition of the monthly and daily primary hydro generation target of area $v$
  Extreme cases are:

    - **A is the identity matrix**  
      The hydro storage energy monthly and weekly profiles of each zone $z$ depend only on the local demand and
      must-run generation in $z$
    - **A has a main diagonal of zeroes**  
      The hydro storage energy monthly and weekly profiles of each zone $z$ do not depend at all on the local
      demand and must-run generation in $z$
- $L_{z.}^+$ Time-series of "net" load for zone $z$, defined as: $L{z.}^+ = L{z.} - M{z.}$
- $L_{z.}$ Time-series of "weighted" load for zone $z$, defined as: $L_{z.} = A^t L_{z.}^+$

<ins>All following parameters are related to the generic zone $z$:</ins>

- $\alpha$ "inter-monthly generation breakdown" parameter

- $\beta$ "inter-daily generation breakdown" parameter

- $j$ "follow-load" parameter

- $\mu$ "reservoir-management" parameter

- $S$ Reservoir size

- $\overline{S_d}$ Reservoir maximum level at the end of day d (rule curve)

- $\underline{S_d}$ Reservoir minimum level at the end of day d (rule curve)

- $S_0$ Reservoir initial level at the beginning of the first day of the "hydro-year"

- $I_d$ Natural inflow of energy to the reservoir during day d

- $I_m$ Natural inflow of energy to the reservoir during month m (sum of $I_d$

- $\overline{W_d}$ Maximum energy that can be generated on day d (standard credit)

<ins> All following variables, defined for both stages, are related to the generic zone: </ins>

- $S_d^k$ Reservoir level at the end of day d, at the end of stage k of pre-allocation

- $S_m^k$ Reservoir level at the end of month m, at the end of stage k of pre-allocation

- $O_d^k$ Overflow from the reservoir on day d, at the end of stage k of pre-allocation (inflow in excess to an already full reservoir)

- $W_d^k$ Energy to generate on day d, at the end of stage k of pre-allocation

- $W_m^k$ Energy to generate on month m, at the end of stage k of pre-allocation


Following variables and parameters are local to linear optimization problems $M$ &amp; $D(m)$
solved within the heuristic. For the sake of clarity, the same generic index is used for all time steps,
knowing that in $M$ there are 12 monthly time-steps, while in $D(m)$ there are from 28 to 31 daily
time-steps. Costs $\gamma_{Var}$ given to these variables are chosen to enforce a logical hierarchy
of penalties (letting the reservoir overflow is worse than violating rule curves, which is worse than deviating
from the generation objective assessed in stage 1, etc.)

- $W$ Generation deficit at the end of the period, as compared to the objective aimed at (positive varaible)

- $O_t$ Overflow from the reservoir on time step $t$ (positive varaible)

- $G_t, \overline{G_t}, \underline{G_t}$ Optimal generation, maximum and minimum generation on time step $t$

- $T_t$ Generation objective assessed in the first stage, for time step t ( $W_m^1$ or $W_d^1$)

- $S_t, \overline{S_t}, \underline{S_t}$ Optimal stock level, maximum level, minimum level at the end of time step $t$

- $I_t$ Natural inflow on time step $t$

- $D_t$ Deviation (absolute difference) between target reached and initial aim

- $\Delta$ Maximum deviation throughout the period

- $V_t^+$ Amplitude of the violation of the upper rule curve at time step $t$ (positive variable)

- $V_t^-$ Amplitude of the violation of the lower rule curve at time step $t$ (positive variable)

- $Y$ Maximum violation of lower rule curve throughout the period


**General heuristic for each zone**

_Begin_

$$if (not.\mu) : \{ S \leftarrow \infty ; \underline{S_d} \leftarrow 0; \overline{S_d}; S_0 \leftarrow S/2 \}$$

_M1:_

$$if (j \text{ and } \mu) : \text{for } m\in [1, 12]: W_m^1 \leftarrow \frac{L_m^{\alpha}.(\sum_{m}{I_m})}{(\sum_{m}{L_m^{\alpha}})}$$

$$else: \text{for } m\in [1, 12]: \{W_m^1 \leftarrow I_m\}$$


_M2:_

$$if (\mu) : \text{for } m\in [1, 12]: W_m^2 \leftarrow \text{Solution of linear problem M}$$

$$else : W_m^2 \leftarrow W_m^1$$

_D1:_

$$if (j): \text{for } d\in [1, 31]: W_d^1 \leftarrow \frac{L_d^{\beta}. (W_m^2)}{(\sum_{d\in m}{L_d^{\beta}})}$$

$$else: \text{for } d\in [1, 31]: W_d^1 \leftarrow I_d $$

_D2:_

$$if(\mu) : \text{for } m \in [1, 12]: W_{d\in m}^2 \leftarrow \text{Solution of linear problem D(m)}$$
$ else : \text{for } m \in [1, 12]: W_{d\in m}^2 \leftarrow$ Solution of a simplified version of linear problem $D(m)$ without reservoir levels

_End_

Note: In the formulation of the optimal hydro-thermal unit-commitment and dispatch problem (see dedicated document), the reference hydro energy __HIT__ used to set the right hand sides of hydro- constraints depends on the value chosen for the optimization preference "simplex range" and is defined as follows:

- Daily : for each day **d** of week $\omega$ : $HIT = W_d^2$
- Weekly : for week $\omega$: $HIT = \sum_{d\in \omega}{W_d^2}$

**Optimization problem M**

$$\min_{G_t, S_t, ...}{\gamma_{\Delta}\Delta + \gamma_Y Y + \sum_{t}{(\gamma_D D_t + \gamma_{V+} V_t^+ + \gamma_{V-} V_t^-)}}$$

s.t

$$S_t \geq 0$$

$$S_t \leq S$$

$G_t \geq \underline{G_t} $ and $G_t \leq \overline{G_t} $

For $t\in [1,12], S_{t} + G_{t} - S_{t-1} = I_{t}$ (see note [^monthly_allocation]) and $S_{12} = S_{0}.$ 

$$G_t - D_t \leq T_t$$

$$G_t + D_t \geq T_t$$

$$V_t^- + S_t \geq \underline{S_t}$$

$$V_t^+ - S_t \geq -\overline{S_t}$$

$$Y - V_t^- \geq 0$$

$$\Delta - D_t \geq 0$$

**Optimization problems $D(m)$**

$$\min_{G_t, S_t, ...}{\gamma_{\Delta}\Delta + \gamma_Y Y + \gamma_{W}W+ \sum_{t}{(\gamma_D D_t + \gamma_{V-} V_t^- + \gamma_{O} O_t + \gamma_S S_t)}}$$
s.t

$$S_t \geq 0$$

$$S_t \leq S$$

$$G_t \geq \underline{G_t}$$

$$G_t \leq \overline{G_t}$$

$S_t + G_t + O_t - S_{t-1} = I_t$ (see note [^daily_allocation]) (initial level of the period is either $S_0$ if $m=1$ or the final level found in solving $D(m-1)$)

$\sum_{t}{G_t + W} = \sum_{t}{T_t} + W_{m-1}$ (0 if $m=1$ else value of $W$ previously found in solving **$D(m-1)$**)

$$G_t - D_t \leq T_t + \frac{W_{m-1}}{|d \in m|}$$

$$G_t + D_t \geq T_t + \frac{W_{m-1}}{|d \in m|}$$

$$\Delta - D_t \geq 0$$

$$V_t^- + S_t \geq \underline{S_t}$$

$$Y - V_t^- \geq 0$$

[^monthly_allocation]: In the first equation, $S_{t-1}$ is equal to initial stock $S_0$

[^daily_allocation]: In the first equation, $S_{t-1}$ is either the initial stock used in M or the final stock of the previous month ($D(m-1)$)
