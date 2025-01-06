FROM --platform=linux/amd64 gcc:7

# Install required packages
RUN apt-get update && apt-get install -y \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libglfw3-dev \
    libglew-dev \
    freeglut3-dev \
    flex \
    bison \
    mesa-common-dev \
    libgl-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Build the project
RUN make -f Makefile.x11

# Set the default command
CMD ["./build/QuantumGL"]