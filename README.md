# COVID-19 X-Ray Classification

A Qt application for COVID-19 detection from X-ray images using deep learning.

## Prerequisites

- C++17 compatible compiler
- CMake 3.16 or higher
- Qt 6.x (or Qt 5.12+)
- OpenCV 4.x
- Python 3.x (for model conversion)

## Setup Instructions

### macOS

1. Install dependencies:
   ```bash
   brew install cmake opencv qt@6
   ```

2. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/MyQtApp.git
   cd MyQtApp
   ```

3. Build the project:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

4. Run the application:
   ```bash
   ./MyQtApp.app/Contents/MacOS/MyQtApp
   ```

### Windows

1. Install dependencies:
   - [CMake](https://cmake.org/download/)
   - [Qt 6.x](https://www.qt.io/download)
   - [OpenCV 4.x](https://opencv.org/releases/)
   - [Visual Studio](https://visualstudio.microsoft.com/) with C++ components or MinGW

2. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/MyQtApp.git
   cd MyQtApp
   ```

3. Build the project:
   ```bash
   mkdir build && cd build
   cmake .. -DQt6_DIR="C:/Qt/6.9.0/msvc2019_64/lib/cmake/Qt6" -DOpenCV_DIR="C:/path/to/opencv/build"
   cmake --build . --config Release
   ```

4. Run the application:
   ```bash
   ./Release/MyQtApp.exe
   ```

## Dataset

The COVID-19 radiography database should be placed in the `data/` directory with the following structure:
```
data/
  COVID/images/
  Lung_Opacity/images/
  Normal/images/
  Viral Pneumonia/images/
```

You can download the dataset from [Kaggle](https://www.kaggle.com/tawsifurrahman/covid19-radiography-database).

## Model Conversion

To convert the Keras model to the format usable by frugally-deep:

```bash
python convert_model.py epoch_30.h5 epoch_30.json
```

## Development Guidelines

- Create feature branches from `main`
- Submit pull requests for code review
- Keep the code well-documented
- Run tests before pushing changes
