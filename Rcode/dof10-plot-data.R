#set the working directory
#setwd("~/Documents/Arduino/sketches/DOf10api/Rcode")
#setwd("/Volumes/MBH")

#read in the data to a dataframe df
df<-read.csv("../data/dof10data/test2.csv",stringsAsFactors=FALSE)
id<-seq(1,nrow(df))
df<-cbind(id,df)

#rename the columns
names(df)<-c("id","unixTime","timeStamp","xg","yg","zg","xa","ya","za")
library(lubridate)
df$timeStamp<-ymd_hms(df$timeStamp)


#seconds since began
df$tInterp=(df$id/max(df$id))*(df$timeStamp[nrow(df)]-df$timeStamp[1])

#check the data
str(df)
print(summary(df))

#plot the data
index=500:2010#nrow(df) #change this if necessary
library(rafalib)
mypar(3,1)
plot(df$tInterp[index],df$xa[index],type="l",xlab="time (s)",ylab="x accel (m/s2)",ylim=c(min(min(df$xa,df$xg)),max(max(df$xa,df$xg))))
#lines(df$xg[index],type="l",col="blue")

plot(df$tInterp[index],df$ya[index],type="l",xlab="time (s)",ylab="y accel (m/s2)",ylim=c(min(min(df$ya,df$yg)),max(max(df$ya,df$yg))))
#lines(df$yg[index],type="l",col="blue")
plot(df$tInterp[index],df$za[index],type="l",xlab="time (s)",ylab="z accel (m/s2)",ylim=c(min(min(df$za,df$zg)),max(max(df$za,df$zg))))
#lines(df$zg[index],type="l",col="blue")

mypar(1,1)
plot(df$tInterp[index],df$xg[index],xlab="time (s)",ylab="Proportional to acceleration",type="l",col="blue")
text(20,-3,"Walking")
text(37,-10,"Cantering")
#plot(df$tInterp[index],df$yg[index],xlab="time (s)",ylab="y rot (rad/s)",type="l",col="blue")
#plot(df$tInterp[index],df$zg[index],xlab="time (s)",ylab="z rot (rad/s)",type="l",col="blue")
#abline(4,0,col="red")

#reset wd so can eject SD card
#setwd("/Volumes")