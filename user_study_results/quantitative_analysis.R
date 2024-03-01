#install packages
library(reshape2)
install.packages("stats")
library(stats)

#Give the correct path and change the filename
myData<-read.csv("/Users/divya/Documents/Data_collection/userstudy/q1.csv", sep=",", na.strings=c("", "NA"))

myData2<-myData 
myData2$ID<-seq.int(nrow(myData2))

#Read as a matrix:
myMatrix<-data.matrix(myData)

#In long format
myDataLong<-melt(myData2, id.vars=c("ID"))

#FRIEDMAN TEST

#friedman.test(myMatrix)
friedman.test(myDataLong$value,myDataLong$variable, myDataLong$ID)

#WILCOXON POST-HOC FRIEDMAN TEST

pairwise.wilcox.test(myDataLong$value, myDataLong$variable, paired=TRUE, exact=FALSE, p.adj="none", correct=FALSE)

#EFFECT SIZE

kendall_result <- cor.test(myDataLong$value, myDataLong$ID, method="kendall")
cat("Kendall's W:", kendall_result$estimate, "\n")

#Using the agricolae package:
#install.packages("agricolae")
library(agricolae)
friedman(myDataLong$ID,myDataLong$variable,myDataLong$value,console=TRUE)