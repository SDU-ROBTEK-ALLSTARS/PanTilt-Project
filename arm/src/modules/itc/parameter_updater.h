
#ifndef PARAMETER_UPDATER_H_
#define PARAMETER_UPDATER_H_

#define PAR_UPDATER_TASK_STACK_SIZE  (configMINIMAL_STACK_SIZE)
#define PAR_UPDATER_TASK_PRIORITY    (tskIDLE_PRIORITY + 1)
#define PAR_UPDATER_TASK_FREQUENCY   ((portTickType) (5 / portTICK_RATE_MS))

BOOLEAN par_updater_init(void);

#endif /* PARAMETER_UPDATER_H_ */
