package com.example.e7

import kotlinx.coroutines.*

class MatrixMultiplier(private val matrixSize: Int, private val cpus: Int) {
    private var a: DoubleArray = DoubleArray(matrixSize * matrixSize) { 1.0 }
    private var b: DoubleArray = DoubleArray(matrixSize * matrixSize) { 1.0 }
    private var c: DoubleArray = DoubleArray(matrixSize * matrixSize) { 0.0 }

    private suspend fun multiply() = coroutineScope {
        val chunkSize = matrixSize / cpus
        val jobs = List(cpus) { cpu ->
            launch(Dispatchers.Default) {
                val startRow = cpu * chunkSize
                val endRow = if (cpu == cpus - 1) matrixSize else startRow + chunkSize
                for (i in startRow until endRow) {
                    for (k in 0 until matrixSize) {
                        val a_ik = a[i * matrixSize + k]
                        for (j in 0 until matrixSize) {
                            c[i * matrixSize + j] += a_ik * b[k * matrixSize + j]
                        }
                    }
                }
            }
        }
        jobs.forEach { it.join() }
    }

    suspend fun calculate(): Double {
        resetMatrices()

        val startTime = System.nanoTime()
        multiply()
        return (System.nanoTime() - startTime) / 1e9 // Convert nanoseconds to seconds
    }

    private fun resetMatrices() {
        a.fill(1.0)
        b.fill(1.0)
        c.fill(0.0)
    }
}
