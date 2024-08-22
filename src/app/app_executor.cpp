#include "app.h"

#include <signal.h>

/// @brief STDOUT global constant
static HANDLE appExecutorStdout = NULL;

// @brief STDIN global constant
static HANDLE appExecutorStdin = NULL;

//----------------------------------------------------------------
//! @brief signal handling function
//!
//! @param [in] signal signal index
//! 
//! @note writes crash report to stdout
//----------------------------------------------------------------
static void
appSignalHandler( int signal ) {
  AppExecutorTaskStatus taskStatus = APP_EXECUTOR_TASK_STATUS_CRASHED;
  AppExecutorCrashReport crashReport = {
    .signal = signal,
  };

  WriteFile(appExecutorStdout, &taskStatus, sizeof(taskStatus), NULL, NULL);
  WriteFile(appExecutorStdout, &crashReport, sizeof(crashReport), NULL, NULL);
} // appSignalHandler function end

int
appExecutorMain( int argc, const char **argv ) {
  appExecutorStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  appExecutorStdin = GetStdHandle(STD_INPUT_HANDLE);

  // setup signal handlers
  signal(SIGINT  , appSignalHandler);
  signal(SIGILL  , appSignalHandler);
  signal(SIGFPE  , appSignalHandler);
  signal(SIGSEGV , appSignalHandler);
  signal(SIGTERM , appSignalHandler);
  signal(SIGBREAK, appSignalHandler);
  signal(SIGABRT , appSignalHandler);

  AppExecutorTaskType taskType = APP_EXECUTOR_TASK_TYPE_QUIT;
  const AppExecutorTaskStatus okStatus = APP_EXECUTOR_TASK_STATUS_OK;
  BOOL continueExecution = TRUE;

  while (continueExecution && ReadFile(appExecutorStdin, &taskType, sizeof(taskType), NULL, NULL)) {
    switch (taskType) {
      case APP_EXECUTOR_TASK_TYPE_SOLVE : {
        SqsQuadraticEquationCoefficents coefficents = {0};
        SqsQuadraticSolution solution = {
          .status = SQS_QUADRATIC_SOLVE_STATUS_NO_ROOTS,
        };

        assert(ReadFile(appExecutorStdin, &coefficents, sizeof(coefficents), NULL, NULL));
        sqsSolveQuadratic(&coefficents, &solution);

        WriteFile(appExecutorStdout, &okStatus, sizeof(okStatus), NULL, NULL);
        WriteFile(appExecutorStdout, &solution, sizeof(solution), NULL, NULL);

        break;
      }

      case APP_EXECUTOR_TASK_TYPE_TEST  :
        assert(FALSE);
        break;

      case APP_EXECUTOR_TASK_TYPE_QUIT  :
        continueExecution = FALSE;
        break;
    }
  }

  return 0;
} // appExecutorMain function end

// app_executor.cpp file end
