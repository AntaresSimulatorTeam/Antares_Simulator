* FICO Xpress v9.6.0, Hyper, written 11:31:15, Dec 4, 2025
* Written from user model (presolvestate=0x1400a1)
NAME                  
ROWS
 N  COST                                                    
 E  discrete_generator_candidate.p_max_nb_units_relation    
 E  R2                                                      
 L  R3                                                      
 L  R4                                                      
 L  R5                                                      
 L  R6                                                      
 L  R7                                                      
 L  R8                                                      
 L  R9                                                      
 L  R10                                                     
 L  R11                                                     
 L  R12                                                     
 L  R13                                                     
 L  R14                                                     
 L  R15                                                     
 L  R16                                                     
 L  R17                                                     
 L  R18                                                     
 L  R19                                                     
 L  R20                                                     
COLUMNS
    discrete_generator_candidate.nb_units                     discrete_generator_candidate.p_max_nb_units_relation      -10
    continuous_generator_candidate.p_max                      COST                                                      490
    continuous_generator_candidate.p_max                      R3                                                        -491
    continuous_generator_candidate.p_max                      R5                                                        -491
    continuous_generator_candidate.p_max                      R6                                                        -491
    continuous_generator_candidate.p_max                      R7                                                        -491
    continuous_generator_candidate.p_max                      R8                                                        -35
    continuous_generator_candidate.p_max                      R9                                                        -35
    continuous_generator_candidate.p_max                      R10                                                       -35
    continuous_generator_candidate.p_max                      R11                                                       -35
    continuous_generator_candidate.p_max                      R12                                                       -35
    continuous_generator_candidate.p_max                      R13                                                       -35
    continuous_generator_candidate.p_max                      R14                                                       -35
    continuous_generator_candidate.p_max                      R15                                                       -35
    continuous_generator_candidate.p_max                      R16                                                       -35
    continuous_generator_candidate.p_max                      R17                                                       -35
    continuous_generator_candidate.p_max                      R18                                                       -35
    continuous_generator_candidate.p_max                      R19                                                       -35
    continuous_generator_candidate.p_max                      R20                                                       -35
    discrete_generator_candidate.p_max                        COST                                                      200
    discrete_generator_candidate.p_max                        discrete_generator_candidate.p_max_nb_units_relation      1
    discrete_generator_candidate.p_max                        R3                                                        -491
    discrete_generator_candidate.p_max                        R5                                                        -491
    discrete_generator_candidate.p_max                        R6                                                        -491
    discrete_generator_candidate.p_max                        R7                                                        -491
    discrete_generator_candidate.p_max                        R8                                                        -35
    discrete_generator_candidate.p_max                        R9                                                        -35
    discrete_generator_candidate.p_max                        R10                                                       -35
    discrete_generator_candidate.p_max                        R11                                                       -35
    discrete_generator_candidate.p_max                        R12                                                       -35
    discrete_generator_candidate.p_max                        R13                                                       -35
    discrete_generator_candidate.p_max                        R14                                                       -35
    discrete_generator_candidate.p_max                        R15                                                       -35
    discrete_generator_candidate.p_max                        R16                                                       -35
    discrete_generator_candidate.p_max                        R17                                                       -35
    discrete_generator_candidate.p_max                        R18                                                       -35
    discrete_generator_candidate.p_max                        R19                                                       -35
    discrete_generator_candidate.p_max                        R20                                                       -35
    alpha                                                     COST                                                      1
    alpha                                                     R2                                                        1
    alpha_0                                                   R2                                                        -1
    alpha_0                                                   R3                                                        -1
    alpha_0                                                   R4                                                        -1
    alpha_0                                                   R5                                                        -1
    alpha_0                                                   R6                                                        -1
    alpha_0                                                   R7                                                        -1
    alpha_0                                                   R8                                                        -1
    alpha_0                                                   R9                                                        -1
    alpha_0                                                   R10                                                       -1
    alpha_0                                                   R11                                                       -1
    alpha_0                                                   R12                                                       -1
    alpha_0                                                   R13                                                       -1
    alpha_0                                                   R14                                                       -1
    alpha_0                                                   R15                                                       -1
    alpha_0                                                   R16                                                       -1
    alpha_0                                                   R17                                                       -1
    alpha_0                                                   R18                                                       -1
    alpha_0                                                   R19                                                       -1
    alpha_0                                                   R20                                                       -1
RHS
    RHS                                                       R3                                                        -109200
    RHS                                                       R4                                                        -4000
    RHS                                                       R5                                                        -109200
    RHS                                                       R6                                                        -109200
    RHS                                                       R7                                                        -109200
    RHS                                                       R8                                                        -18000
    RHS                                                       R9                                                        -18000
    RHS                                                       R10                                                       -18000
    RHS                                                       R11                                                       -18000
    RHS                                                       R12                                                       -18000
    RHS                                                       R13                                                       -18000
    RHS                                                       R14                                                       -18000
    RHS                                                       R15                                                       -18000
    RHS                                                       R16                                                       -18000
    RHS                                                       R17                                                       -18000
    RHS                                                       R18                                                       -18000
    RHS                                                       R19                                                       -18000
    RHS                                                       R20                                                       -18000
BOUNDS
 UI BOUND                                                     discrete_generator_candidate.nb_units                     10
 UP BOUND                                                     continuous_generator_candidate.p_max                      1000
 LO BOUND                                                     alpha                                                     -1e+10
 LO BOUND                                                     alpha_0                                                   -1e+10
ENDATA
