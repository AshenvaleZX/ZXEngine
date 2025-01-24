plugins {
    alias(libs.plugins.android.application)
}

android {
    namespace = "com.AshenvaleZX.ZXEngine"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.AshenvaleZX.ZXEngine"
        minSdk = 30
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++17"
            }
        }
    }

    signingConfigs {
        create("release") {
            keyAlias = "key0"
            keyPassword = "123456"
            storeFile = file("../../../Tools/TemplateKeystore.jks")
            storePassword = "123456"
        }
    }

    buildTypes {
        debug {
            isDebuggable = true
            isMinifyEnabled = false
            manifestPlaceholders["ZXEngineLibraryName"] = "ZXEngined"
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt")
            )
        }
        release {
            isDebuggable = false
            isMinifyEnabled = false
            manifestPlaceholders["ZXEngineLibraryName"] = "ZXEngine"
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt")
            )
            signingConfig = signingConfigs.getByName("release")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    buildFeatures {
        prefab = true
        buildConfig = true
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
}

dependencies {
    implementation(libs.appcompat)
    implementation(libs.material)
    implementation(libs.games.activity)
}