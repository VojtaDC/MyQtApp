import numpy as np
import matplotlib.pyplot as plt
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Input, Dropout, Flatten, Dense
from tensorflow.keras.applications import Xception
from tensorflow.keras.optimizers import Adam

# Constants
IMG_SIZE = (299, 299)
BATCH_SIZE = 16
EPOCHS = 30
LEARNING_RATE = 1e-3
DATA_DIR = "data/"

# Function to prepare data
def prepare_data(data_dir, img_size, batch_size):
    datagen = ImageDataGenerator(
        rescale=1./255,
        shear_range=0.2,
        zoom_range=0.2,
        validation_split=0.25,
        horizontal_flip=True
    )
    train_data = datagen.flow_from_directory(
        data_dir,
        target_size=img_size,
        batch_size=batch_size,
        class_mode='categorical',
        subset='training'
    )
    val_data = datagen.flow_from_directory(
        data_dir,
        target_size=img_size,
        batch_size=batch_size,
        class_mode='categorical',
        subset='validation'
    )
    class_names = list(train_data.class_indices.keys())
    return train_data, val_data, class_names

# Function to plot images
def plot_images(images_arr, labels, class_names):
    fig, axes = plt.subplots(6, 4, figsize=(20, 30))
    axes = axes.flatten()
    for img, label, ax in zip(images_arr, labels, axes):
        ax.imshow(img)
        ax.set_title(class_names[np.argmax(label)])
        ax.axis('off')
    plt.tight_layout()
    plt.show()

# Function to create the model
def create_model(input_shape, num_classes, learning_rate):
    base = Xception(weights="imagenet", input_shape=input_shape, include_top=False)
    base.trainable = False
    model = Sequential([
        Input(shape=input_shape),
        base,
        Dropout(0.2),
        Flatten(),
        Dropout(0.2),
        Dense(16),
        Dense(num_classes, activation='softmax')
    ])
    model.compile(
        optimizer=Adam(learning_rate=learning_rate),
        loss="categorical_crossentropy",
        metrics=["accuracy"]
    )
    return model

# Prepare data
train, val, class_names = prepare_data(DATA_DIR, IMG_SIZE, BATCH_SIZE)

# Plot sample images
images, labels = train[0]
plot_images(images[:24], labels[:24], class_names)

# Create and summarize the model
model = create_model((*IMG_SIZE, 3), len(class_names), LEARNING_RATE)
model.summary()

# Train the model
model.fit(train, epochs=EPOCHS, validation_data=val)

# Save the model
model.save('epoch_30.h5')
