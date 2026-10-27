# AI Usage Declaration

## Tool and session

- **AI tool:** GitHub Copilot in Visual Studio Code (Copilot SDK).
- **Model:** The specific model identifier was not exposed in the assistant interface and therefore cannot be reliably identified.
- **Date:** October 27, 2026.
- **Time:** Approximately 8:01 PM–11:04 PM Vietnam time (UTC+07:00). The work and learning occurred over multiple interactions during this session.

## Purpose of use

The student used the AI as a tutor and debugging assistant while learning the existing C++ game project and developing its SFML user interface. Assistance covered:

- Explaining the repository layout, the roles of the controller, engine, UI, and debug logger, and how the application starts.
- Explaining the existing board representation, engine state, move placement, and how the UI can read the latest board.
- Introducing SFML 3.1 window creation, event polling, drawing, coordinate mapping, and rendering circles for the stones.
- Giving ordered implementation steps and explaining C++ declarations, data types, and class access (`public` and `private`) when requested.
- Reviewing the student's UI code, explaining compile/runtime issues, and suggesting fixes for function placement, mouse-button handling, screen-to-board coordinate conversion, and consistent board spacing.
- Helping configure CMake to locate and link SFML, configure the local run/debug setup, and copy required SFML runtime DLLs next to the executable.
- Explaining build and runtime errors and checking the resulting build after changes.

## Student learning, authorship, and verification

The AI was used to help the student understand the code rather than only to produce a finished result. The student asked follow-up questions, studied the explanations, applied and adapted the suggested steps in the project, and checked the behavior by rebuilding and running the application. In particular, the student developed the board-stone rendering and mouse-input logic while using AI explanations and review to understand the relationship between engine coordinates and screen pixels.

The AI initially provided a basic SFML window scaffold and proposed code examples for later steps. The student reviewed and incorporated or adapted assistance while working through the implementation. Accordingly, the UI work should not be described as either entirely AI-generated or entirely unaided student work. The student is responsible for understanding, testing, and validating the submitted code.

## Scope and limitations

The AI did not independently verify the full game rules or certify the entire project. Some guidance was based on the source files and build/runtime output available during the session. The exact AI model identifier was unavailable, so no model name is claimed.
