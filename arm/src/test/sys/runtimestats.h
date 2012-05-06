
#ifndef RUNTIMESTATS_H
#define RUNTIMESTATS_H

#define TIMER0_CONF_FREQUENCY                    (configCPU_CLOCK_HZ / (configTICK_RATE_HZ * 40))
#define RUNTIMESTATS_REQUEST_INSTRUCTION_NUMBER  1

BOOLEAN runtimestats_init(void);
void timer0_config_hw(void);

#endif /* RUNTIMESTATS_H */
